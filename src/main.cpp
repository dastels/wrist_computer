// -*- mode: c++ -*-

// Main code
//
// The MIT License (MIT)
//
// Copyright (c) 2021 Dave Astels
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// System includes
#include <Arduino.h>
#include <seesaw_neopixel.h>
#include <LibPrintf.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SdFat.h>
#include <WiFiNINA.h>

// Sensor includes
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_BME680.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_ADT7410.h>

// Screen includes
#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>
#include <TouchScreen.h>

// Local includes
#include "defines.h"
#include "secrets.h"
#include "indicator.h"
#include "logging.h"
#include "serial_handler.h"
#include "sdcard_handler.h"
#include "null_handler.h"
#include "haptic.h"
#include "sensor_readings.h"
#include "eeprom.h"
#include "encoder.h"

// App includes
#include "idle_screen.h"
#include "app.h"
#include "alarm.h"
#include "stopwatch.h"
#include "timer.h"
#include "weather.h"
#include "compass.h"
#include "settings.h"

LV_FONT_DECLARE(DSEG7_16);
LV_FONT_DECLARE(DSEG7_32);

// ILI9341 with 8-bit parallel interface:
Adafruit_ILI9341 tft(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
Adafruit_SPIFlash flash(&flashTransport);
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();
RTC_DS3231 rtc;
seesaw_NeoPixel ss(8, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303DLH_Mag_Unified mag = Adafruit_LSM303DLH_Mag_Unified(12345);
Adafruit_BME680 bme;
Haptic haptic;

Eeprom eeprom(&ss);
bool silent = false; //eeprom.get_silent();

uint32_t button_pin_mask = (1 << CENTER_BUTTON_PIN) | (1 << CANCEL_BUTTON_PIN) | (1 << UP_BUTTON_PIN) | (1 << DOWN_BUTTON_PIN) | (1 << LEFT_BUTTON_PIN) | (1 << RIGHT_BUTTON_PIN);

Indicator pixels[8] = {Indicator(&ss, 0), Indicator(&ss, 1), Indicator(&ss, 2), Indicator(&ss, 3), Indicator(&ss, 4), Indicator(&ss, 5), Indicator(&ss, 6), Indicator(&ss, 7)};

SensorReadings sensor_readings;
App *current_app = nullptr;
IdleScreen *idle;

Logger *logger;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define SEALEVELPRESSURE_HPA (1013.25)

TouchScreen ts(XP, YP, XM, YM, 300);
Adafruit_LvGL_Glue glue;

SdFat SD;

char strbuf[4096];


//==============================================================================
// Audio support

// AudioPlayMemory          sample1;
// AudioOutputAnalogStereo  audioOutput;
// AudioConnection          patchCord1(sample1, 0, audioOutput, 1);
// AudioConnection          patchCord2(sample1, 1, audioOutput, 0);


void fail(void)
{
  int delta = 1;
  byte red = 0;
  while (1) {
    pixels[0].show(red, 0, 0);
    if (red == 255 || red == 0) {
      delta *= -1;
    }
    red += delta;
    delay(75);
  }
}


void initialize_serial()
{
  Serial.begin(115200);
  while (!Serial);
  delay(1000);
}


void initialize_logging()
{
  LoggingHandler *handler;
  // LoggingHandler *sd_handler = new SDCardHandler(SD_CS);
  if (LOG_TO_SERIAL) {
    initialize_serial();
    handler = new SerialHandler();
  } else {
    handler = new NullHandler();
  }
  logger = Logger::get_logger(handler);
  logger->set_level(log_level_for(LOG_LEVEL));
//   if (sd_handler->initialized()) {
//     logger->add_handler(sd_handler);
//   }
}


//==============================================================================
// Display update tasks
//==============================================================================
lv_task_t * time_update_task;
lv_task_t * light_update_task;
lv_task_t * temp_update_task;
lv_task_t * accel_update_task;
lv_task_t * mag_update_task;
lv_task_t * bme_update_task;
lv_task_t * buttons_update_task;
lv_task_t * display_update_task;

lv_task_t * app_update_task;
//------------------------------------------------------------------------------

//==============================================================================
// Haptic patterns
//==============================================================================
uint8_t stopwatch_start_haptic;
uint8_t stopwatch_stop_haptic;
uint8_t timer_ring_haptic;

void initialize_haptic_patterns()
{
  uint8_t stopwatch_start_pattern[] = {24, 0};
  stopwatch_start_haptic = haptic.add_effect(stopwatch_start_pattern);

  uint8_t stopwatch_stop_pattern[] = {34, 0};
  stopwatch_stop_haptic = haptic.add_effect(stopwatch_stop_pattern);

  uint8_t timer_ring_pattern[] = {37, 123, 123, 37, 123, 123, 37, 123, 123, 37, 0};
  timer_ring_haptic = haptic.add_effect(timer_ring_pattern);
}
//------------------------------------------------------------------------------


void update_time_display(lv_task_t *task)
{
  DateTime t = rtc.now();
  current_app->update_time_display(&t);
}


void update_app(lv_task_t *task)
{
  unsigned long now = millis();
  idle->update_persistant_background_apps(now);
  if (current_app) {
    current_app->update(now);
  }
}


bool is_current_app(App *app)
{
  return app == current_app;
}


uint32_t wheel(byte wheel_pos)
{
  wheel_pos = 255 - wheel_pos;
  if (wheel_pos < 85) {
    return ss.Color(255 - wheel_pos * 3, 0, wheel_pos * 3);
  }
  if (wheel_pos < 170) {
    wheel_pos -= 85;
    return ss.Color(0, wheel_pos * 3, 255 - wheel_pos * 3);
  }
  wheel_pos -= 170;
  return ss.Color(wheel_pos * 3, 255 - wheel_pos * 3, 0);
}


void init_hardware()
{
  bool success = true;

  // RTC
  if (!rtc.begin()) {
    logger->debug("RTC FAILED");
    success = false;
  } else {
    logger->debug("RTC PASSED");
  }
  lv_task_handler();

  // SEESAW
  if (!ss.begin(0x36)) {
    logger->debug("SEESAW FAILED");
    success = false;
  } else {
    logger->debug("SEESAW PASSED");
    ss.pinModeBulk(button_pin_mask, INPUT_PULLUP);
    ss.show();
  }
  lv_task_handler();

  // LSM303
  if (!accel.begin() || !mag.begin()) {
    logger->debug("LSM303 FAILED");
    success = false;
  } else {
    logger->debug("LSM303 PASSED");
    mag.setMagRate(LSM303_MAGRATE_220);
  }
  lv_task_handler();

  // BME680
  if (!bme.begin()) {
    logger->debug("BME680 FAILED");
    success = false;
  } else {
    logger->debug("BME680 PASSED");
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms
  }
  lv_task_handler();

  // QSPI FLASH
  if (!flash.begin()){
    logger->debug("QSPI FLASH FAILED");
    fail();
  } else {
    logger->debug("QSPI FLASH PASSED");
  }
  lv_task_handler();

  // SD CARD
  if (!SD.begin(SD_CS)) {
    logger->debug("SD NOT PRESENT");
  } else {
    logger->debug("SD PRESENT");
  }
  lv_task_handler();

  // WiFi Module
  WiFi.status();
  delay(100);
  if (WiFi.status() == WL_NO_MODULE) {
    logger->debug("WiFi FAILED");
    success = false;
  } else {
    logger->debug("WiFi PASSED");
  }
  lv_task_handler();

  // PyPortal Temperature sensor
  if (!tempsensor.begin()) {
    logger->debug("ADT7410 FAILED");
    success = false;
  } else {
    logger->debug("ADT7410 PASSED");
  }
  lv_task_handler();

  // Haptic
  if (!haptic.begin()) {
    logger->debug("DRV2605L FAILED");
    success = false;
  } else {
    logger->debug("DRV2605L PASSED");
    initialize_haptic_patterns();
  }
  lv_task_handler();

  initialize_nav_pad();

  if (!success) {
    fail();
  }
}


//==============================================================================
// Task functions
//==============================================================================


void update_light(lv_task_t *task)
{
  sensor_readings.light = analogRead(LIGHT_SENSOR);
}


void update_internal_temperature(lv_task_t *task)
{
  sensor_readings.internal_temperature = tempsensor.readTempC();
}


void update_acceleration(lv_task_t *task)
{
  sensors_event_t event;
  accel.getEvent(&event);
  sensor_readings.acceleration.x = event.acceleration.x;
  sensor_readings.acceleration.y = event.acceleration.y;
  sensor_readings.acceleration.z = event.acceleration.z;
}


void update_magnetic(lv_task_t *task)
{
  sensors_event_t event;
  mag.getEvent(&event);
  sensor_readings.magnetic.x = event.magnetic.x;
  sensor_readings.magnetic.y = event.magnetic.y;
  sensor_readings.magnetic.z = event.magnetic.z;
}


void update_bme(lv_task_t *task)
{
  if (bme.performReading()) {
    sensor_readings.temperature = bme.temperature;
    sensor_readings.pressure = bme.pressure / 100.0;
    sensor_readings.humidity = bme.humidity;
    sensor_readings.gas = bme.gas_resistance / 1000.0;
    sensor_readings.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  }
}

void init_sensor_tasks()
{
  static uint32_t user_data = 10;

  light_update_task = lv_task_create(update_light, 2000, LV_TASK_PRIO_LOW, &user_data);
  lv_task_ready(light_update_task);

  temp_update_task = lv_task_create(update_internal_temperature, 10000, LV_TASK_PRIO_LOW, &user_data);
  lv_task_ready(temp_update_task);

  accel_update_task = lv_task_create(update_acceleration, 500, LV_TASK_PRIO_HIGH, &user_data);
  lv_task_ready(accel_update_task);

  mag_update_task = lv_task_create(update_magnetic, 5000, LV_TASK_PRIO_LOW, &user_data);
  lv_task_ready(mag_update_task);

  bme_update_task = lv_task_create(update_bme, 6000, LV_TASK_PRIO_LOW, &user_data);
  lv_task_ready(bme_update_task);

}


void disable_sensor_tasks()
{
  lv_task_set_prio(light_update_task, LV_TASK_PRIO_OFF);
  lv_task_set_prio(temp_update_task, LV_TASK_PRIO_OFF);
  lv_task_set_prio(accel_update_task, LV_TASK_PRIO_OFF);
  lv_task_set_prio(mag_update_task, LV_TASK_PRIO_OFF);
  lv_task_set_prio(bme_update_task, LV_TASK_PRIO_OFF);
}


void enable_sensor_tasks()
{
  lv_task_set_prio(light_update_task, LV_TASK_PRIO_LOW);
  lv_task_set_prio(temp_update_task, LV_TASK_PRIO_LOW);
  lv_task_set_prio(accel_update_task, LV_TASK_PRIO_HIGH);
  lv_task_set_prio(mag_update_task, LV_TASK_PRIO_LOW);
  lv_task_set_prio(bme_update_task, LV_TASK_PRIO_LOW);
}


// void printWifiStatus() {

//   // print the SSID of the network you're attached to:
//   logger->debug("SSID: %s", WiFi.SSID());

//   // print your board's IP address:
//   IPAddress ip = WiFi.localIP();
//   logger->debug("IP Address: ");

//   Serial.println(ip);

//   // print the received signal strength:
//   logger->debug("signal strength (RSSI): %f dBm", WiFi.RSSI());
// }


void setup() {
  logger->debug("Setup()");

  initialize_logging();

  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  pinMode(TFT_RESET, OUTPUT);
  digitalWrite(TFT_RESET, HIGH);
  delay(10);
  digitalWrite(TFT_RESET, LOW);
  delay(10);
  digitalWrite(TFT_RESET, HIGH);
  delay(10);

  // Initialize display BEFORE glue setup
  tft.begin();
  tft.setRotation(TFT_ROTATION);
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextWrap(true);

    // Initialize glue, passing in address of display & touchscreen
  LvGLStatus status = glue.begin(&tft, &ts);

  if(status != LVGL_OK) {
    Serial.print("Glue error "); Serial.println((int)status);
    while(1);
  }

  lv_obj_t * preload = lv_spinner_create(lv_scr_act(), NULL);
  lv_obj_set_size(preload, 100, 100);
  lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);


  init_hardware();
  lv_task_handler();

  int wifi_status = WL_IDLE_STATUS;
  while (wifi_status != WL_CONNECTED) {
    logger->debug("Attempting to connect to SSID: %s", ssid);
    wifi_status = WiFi.begin(ssid, pass);
    for (uint8_t i = 0; i < 200; i++) {
      lv_task_handler();
      delay(50);               // wait 10 seconds for connection
    }
  }

  logger->debug("Connected to wifi");
  //  printWifiStatus();


  pinMode(SPKR_ENABLE, OUTPUT);
  digitalWrite(SPKR_ENABLE, LOW);

  // Cleanup status display
  // lv_obj_del(status_text);
  // lv_obj_del(label);

  idle = new IdleScreen();
  idle->register_app(new Alarm());
  idle->register_app(new Stopwatch());
  idle->register_app(new Timer());
  idle->register_app(new Weather());
  idle->register_app(new Compass());
  idle->register_app(new Settings());
  current_app = idle;
  idle->activate();

  logger->debug("Idle activated");

  static uint32_t user_data = 10;
  app_update_task = lv_task_create(update_app, 10, LV_TASK_PRIO_HIGHEST, &user_data);
  lv_task_ready(app_update_task);

  time_update_task = lv_task_create(update_time_display, 1000, LV_TASK_PRIO_HIGH, &user_data);
  lv_task_ready(time_update_task);

  init_sensor_tasks();

  logger->debug("Tasks created");

  bool silent = eeprom.get_silent();

}


void loop() {
  lv_task_handler(); // Call LittleVGL task handler periodically
  delay(2);
}
