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

#include <Arduino.h>
#include <seesaw_neopixel.h>
#include <LibPrintf.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SdFat.h>
#include <WiFiNINA.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_BME680.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_ADT7410.h>

#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>
#include <TouchScreen.h>

#include "indicator.h"
#include "logging.h"
#include "serial_handler.h"
#include "sdcard_handler.h"
#include "null_handler.h"
#include "haptic.h"

#define SEESAW_CENTER_BUTTON_PIN (24)
#define SEESAW_UP_BUTTON_PIN      (3)
#define SEESAW_DOWN_BUTTON_PIN    (2)
#define SEESAW_LEFT_BUTTON_PIN   (11)
#define SEESAW_RIGHT_BUTTON_PIN  (25)
#define SEESAW_NEOPIXEL_PIN      (15)

#define RED_LED       (13)
#define TFT_RESET     (24)
#define TFT_BACKLIGHT (25)
#define LIGHT_SENSOR  (A2)
#define SD_CS         (32)
#define SPKR_SHUTDOWN (50)

#define TFT_ROTATION   (1) // Landscape orientation on PyPortal
#define TFT_D0        (34) // Data bit 0 pin (MUST be on PORT byte boundary)
#define TFT_WR        (26) // Write-strobe pin (CCL-inverted timer output)
#define TFT_DC        (10) // Data/command pin
#define TFT_CS        (11) // Chip-select pin
#define TFT_RST       (24) // Reset pin
#define TFT_RD         (9) // Read-strobe pin
#define TFT_BACKLIGHT (25)

// ILI9341 with 8-bit parallel interface:
Adafruit_ILI9341 tft(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
Adafruit_SPIFlash flash(&flashTransport);
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();
RTC_DS3231 rtc;
seesaw_NeoPixel ss(8, SEESAW_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303DLH_Mag_Unified mag = Adafruit_LSM303DLH_Mag_Unified(12345);
Adafruit_BME680 bme;
Adafruit_DRV2605 haptic;
uint32_t button_pin_mask = 0;
Indicator pixels[8] = {Indicator(&ss, 0), Indicator(&ss, 1), Indicator(&ss, 2), Indicator(&ss, 3), Indicator(&ss, 4), Indicator(&ss, 5), Indicator(&ss, 6), Indicator(&ss, 7)};

SensorReadings sensor_readings;

Logger *logger;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define SEALEVELPRESSURE_HPA (1013.25)

#define YP (A4)  // must be an analog pin, use "An" notation!
#define XM (A7)  // must be an analog pin, use "An" notation!
#define YM (A6)   // can be a digital pin
#define XP (A5)   // can be a digital pin
TouchScreen ts(XP, YP, XM, YM, 300);
Adafruit_LvGL_Glue glue;

#define X_MIN  (750)
#define X_MAX  (325)
#define Y_MIN  (840)
#define Y_MAX  (240)

SdFat SD;

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


// Override printf's putchar
void _putchar(char character)
{
  tft.print(character);
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
  LoggingHandler *sd_handler = new SDCardHandler(SD_CS);
  if (LOG_TO_SERIAL) {
    initialize_serial();
    handler = new SerialHandler();
  } else {
    handler = new NullHandler();
  }
  logger = Logger::get_logger(handler);
  logger->set_level(log_level_for(LOG_LEVEL));
  if (sd_handler->initialized()) {
    logger->add_handler(sd_handler);
  }
}


//==============================================================================
// For sample sensor display
//==============================================================================
lv_obj_t *time_value_label;
lv_obj_t *light_value_label;
lv_obj_t *temp_value_label;
lv_obj_t *accel_value_label;
lv_obj_t *mag_value_label;
lv_obj_t *bme1_value_label;
lv_obj_t *bme2_value_label;
lv_obj_t *buttons_value_label;
//------------------------------------------------------------------------------

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
//------------------------------------------------------------------------------

//==============================================================================
// Task functions
//==============================================================================

char buffer[128]; // Since the LVGL formatting doesn't seem to support floats

void update_time_display(lv_task_t *task)
{
  DateTime t = rtc.now();
  sprintf(buffer, "%4d/%02d/%02d %02d:%02d:%02d", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());
  lv_label_set_text(time_value_label, buffer);
}

void update_light_display(lv_task_t *task)
{
  sprintf(buffer, "%3d", analogRead(LIGHT_SENSOR));
  lv_label_set_text(light_value_label, buffer);
}

void update_temp_display(lv_task_t *task)
{
  sprintf(buffer, "%5.2fC", tempsensor.readTempC());
  lv_label_set_text_fmt(temp_value_label, buffer);
}

void update_accel_display(lv_task_t *task)
{
  sensors_event_t event;
  accel.getEvent(&event);
  sprintf(buffer, "(%.2f, %.2f, %.2f) m/s^2", event.acceleration.x, event.acceleration.y, event.acceleration.z);
  lv_label_set_text(accel_value_label, buffer);
}

void update_mag_display(lv_task_t *task)
{
  sensors_event_t event;
  mag.getEvent(&event);
  sprintf(buffer, "(%.2f, %.2f, %.2f) uT", event.magnetic.x, event.magnetic.y, event.magnetic.z);
  lv_label_set_text(mag_value_label, buffer);
}

void update_bme_display(lv_task_t *task)
{
  if (bme.performReading()) {
    sprintf(buffer, "%.2fC, %.2f hPa, %.2f%%", bme.temperature, bme.pressure / 100.0, bme.humidity);
    lv_label_set_text(bme1_value_label, buffer);
    sprintf(buffer, "%.2f kOhms, ~%.2f m", bme.gas_resistance / 1000.0, bme.readAltitude(SEALEVELPRESSURE_HPA));
    lv_label_set_text(bme2_value_label, buffer);
  }
}

void update_buttons_display(lv_task_t *task)
{
  uint32_t buttons = ss.digitalReadBulk(button_pin_mask);
  if (buttons > 0) { // there seems to be some all 0 readings which are nonsensical
    sprintf(buffer, "%s%s%s%s%s",
            (buttons & (1 << SEESAW_CENTER_BUTTON_PIN)) ? "" : "C ",
            (buttons & (1 << SEESAW_UP_BUTTON_PIN)) ? "" : "U ",
            (buttons & (1 << SEESAW_DOWN_BUTTON_PIN)) ? "" : "D ",
            (buttons & (1 << SEESAW_LEFT_BUTTON_PIN)) ? "" : "L ",
            (buttons & (1 << SEESAW_RIGHT_BUTTON_PIN)) ? "" : "R");
    lv_label_set_text(buttons_value_label, buffer);
  }
}


void setup() {
  boolean success = true;

  //  initialize_logging();

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

  printf_init(tft);

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

  lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Hello PyPortal!");
  lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_obj_t * status_text = lv_textarea_create(lv_scr_act(), NULL);
  lv_obj_set_size(status_text, 240, 175);
  lv_obj_align(status_text, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
  lv_textarea_set_text(status_text, "");

  // RTC
  lv_textarea_add_text(status_text, "RTC ");
  if (!rtc.begin()) {
    lv_textarea_add_text(status_text, "FAILED\n");
    success = false;
  } else {
    lv_textarea_add_text(status_text, "PASSED\n");
  }
  lv_task_handler();

  // SEESAW
  lv_textarea_add_text(status_text, "Seesaw ");
  if (!ss.begin(0x36)) {
    lv_textarea_add_text(status_text, "FAILED\n");
    success = false;
  } else {
    button_pin_mask =
      (1 << SEESAW_CENTER_BUTTON_PIN) |
      (1 << SEESAW_UP_BUTTON_PIN) |
      (1 << SEESAW_DOWN_BUTTON_PIN) |
      (1 << SEESAW_LEFT_BUTTON_PIN) |
      (1 << SEESAW_RIGHT_BUTTON_PIN);

    lv_textarea_add_text(status_text, "PASSED\n");
    ss.pinModeBulk(button_pin_mask, INPUT_PULLUP);
    ss.show();
  }
  lv_task_handler();

  // LSM303
  lv_textarea_add_text(status_text, "LSM303 ");
  if (!accel.begin() || !mag.begin()) {
    lv_textarea_add_text(status_text, "FAILED\n");
    success = false;
  } else {
    lv_textarea_add_text(status_text, "PASSED\n");
  }
  lv_task_handler();

  // BME680
  lv_textarea_add_text(status_text, "BME680 ");
  if (!bme.begin()) {
    lv_textarea_add_text(status_text, "FAILED\n");
    success = false;
  } else {
    lv_textarea_add_text(status_text, "PASSED\n");
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms
  }
  lv_task_handler();

  // QSPI FLASH
  lv_textarea_add_text(status_text, "QSPI Flash ");
  if (!flash.begin()){
    lv_textarea_add_text(status_text, "FAILED\n");
    fail();
  } else {
    lv_textarea_add_text(status_text, "PASSED\n");
  }
  lv_task_handler();

  // SD CARD
  lv_textarea_add_text(status_text, "SD Card ");
  if (!SD.begin(SD_CS)) {
    lv_textarea_add_text(status_text, "NOT PRESENT\n");
  } else {
    lv_textarea_add_text(status_text, "PRESENT\n");
  }
  lv_task_handler();

  // WiFi Module
  lv_textarea_add_text(status_text, "WiFi ");
  WiFi.status();
  delay(100);
  if (WiFi.status() == WL_NO_MODULE) {
    lv_textarea_add_text(status_text, "FAILED\n");
    success = false;
  } else {
    lv_textarea_add_text(status_text, "PASSED\n");
  }
  lv_task_handler();

  // PyPortal Temperature sensor
  lv_textarea_add_text(status_text, "ADT7410 ");
  if (!tempsensor.begin()) {
    lv_textarea_add_text(status_text, "FAILED\n");
    success = false;
  } else {
    lv_textarea_add_text(status_text, "PASSED\n");
  }
  lv_task_handler();

  if (!success) {
    fail();
  }

  analogWriteResolution(12);
  analogWrite(A0, 128);
  pinMode(SPKR_SHUTDOWN, OUTPUT);
  digitalWrite(SPKR_SHUTDOWN, LOW);

  // Cleanup status display
  delay(5000);
  lv_obj_del(status_text);
  lv_obj_del(label);

  //==============================================================================
  // Build display elements for sensor display
  //==============================================================================

  time_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(time_value_label, 100, 16);
  lv_label_set_text(time_value_label, "0000/00/00 00:00:00");
  lv_obj_align(time_value_label, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

  // Light
  lv_obj_t *light_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(light_label, 84, 16);
  lv_obj_set_pos(light_label, 0, 21);
  lv_label_set_text(light_label, "Light:");

  light_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(light_value_label, 84, 16);
  lv_obj_set_pos(light_value_label, 90, 21);
  lv_label_set_text(light_value_label, "");

  // PyPortal temperature
  lv_obj_t *temp_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(temp_label, 84, 16);
  lv_obj_set_pos(temp_label, 0, 37);
  lv_label_set_text(temp_label, "Temp:");

  temp_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(temp_value_label, 84, 16);
  lv_obj_set_pos(temp_value_label, 90, 37);
  lv_label_set_text(temp_value_label, "");

  // Accelerometer
  lv_obj_t *accel_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(accel_label, 84, 16);
  lv_obj_set_pos(accel_label, 0, 53);
  lv_label_set_text(accel_label, "Accel:");

  accel_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(accel_value_label, 84, 16);
  lv_obj_set_pos(accel_value_label, 90, 53);
  lv_label_set_text(accel_value_label, "");

  // Magetometer
  lv_obj_t *mag_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(mag_label, 84, 16);
  lv_obj_set_pos(mag_label, 0, 69);
  lv_label_set_text(mag_label, "Mag:");

  mag_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(mag_value_label, 84, 16);
  lv_obj_set_pos(mag_value_label, 90, 69);
  lv_label_set_text(mag_value_label, "");

  // BME temp/humidity/pressure(altitude)/gas
  lv_obj_t *bme_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(bme_label, 84, 16);
  lv_obj_set_pos(bme_label, 0, 85);
  lv_label_set_text(bme_label, "BME:");

  bme1_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(bme1_value_label, 84, 16);
  lv_obj_set_pos(bme1_value_label, 90, 85);
  lv_label_set_text(bme1_value_label, "");

  bme2_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(bme2_value_label, 84, 16);
  lv_obj_set_pos(bme2_value_label, 90, 101);
  lv_label_set_text(bme2_value_label, "");

  // Control pad buttons
  lv_obj_t *buttons_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(buttons_label, 84, 16);
  lv_obj_set_pos(buttons_label, 0, 117);
  lv_label_set_text(buttons_label, "Buttons:");

  buttons_value_label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_size(buttons_value_label, 84, 16);
  lv_obj_set_pos(buttons_value_label, 90, 117);
  lv_label_set_text(buttons_value_label, "");

  lv_task_handler();

  //------------------------------------------------------------------------------
  // Start display update tasks

  static uint32_t user_data = 10;

  time_update_task = lv_task_create(update_time_display, 1000, LV_TASK_PRIO_MID, &user_data);
  lv_task_ready(time_update_task);

  light_update_task = lv_task_create(update_light_display, 1000, LV_TASK_PRIO_LOW, &user_data);
  lv_task_ready(light_update_task);

  temp_update_task = lv_task_create(update_temp_display, 10000, LV_TASK_PRIO_LOW, &user_data);
  lv_task_ready(temp_update_task);

  accel_update_task = lv_task_create(update_accel_display, 500, LV_TASK_PRIO_HIGH, &user_data);
  lv_task_ready(accel_update_task);

  mag_update_task = lv_task_create(update_mag_display, 5000, LV_TASK_PRIO_LOW, &user_data);
  lv_task_ready(mag_update_task);

  bme_update_task = lv_task_create(update_bme_display, 5000, LV_TASK_PRIO_MID, &user_data);
  lv_task_ready(bme_update_task);
}

uint16_t count = 0;

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

int32_t old_pos = 0;

void loop() {
  lv_task_handler(); // Call LittleVGL task handler periodically
  int32_t pos = ss.getEncoderPosition();
  if (pos != old_pos) {
    pixels[0].show(wheel(constrain(abs(pos) * 2, 0, 255)));
  }
  update_buttons_display(nullptr);
}
