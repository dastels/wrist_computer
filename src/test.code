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
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BME680.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_ADT7410.h>
#include <TouchScreen.h>
#include <SdFat.h>
#include <WiFiNINA.h>

#include "indicator.h"
#include "logging.h"
#include "serial_handler.h"
#include "sdcard_handler.h"
#include "null_handler.h"

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
Adafruit_LSM303_Accel_Unified accel(54321);
Adafruit_LSM303_Mag_Unified mag(12345);
Adafruit_BME680 bme;
uint32_t button_pin_mask = 0;
Indicator pixels[8] = {Indicator(&ss, 0), Indicator(&ss, 1), Indicator(&ss, 2), Indicator(&ss, 3), Indicator(&ss, 4), Indicator(&ss, 5), Indicator(&ss, 6), Indicator(&ss, 7)};

Logger *logger;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
#define SEALEVELPRESSURE_HPA (1013.25)

#define YP (A4)  // must be an analog pin, use "An" notation!
#define XM (A7)  // must be an analog pin, use "An" notation!
#define YM (A6)   // can be a digital pin
#define XP (A5)   // can be a digital pin
TouchScreen ts(XP, YP, XM, YM, 300);
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

  tft.begin();
  tft.setRotation(1);

  printf_init(tft);

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextWrap(true);

  // RTC
  tft.setCursor(0, 0);
  tft.print("RTC ");
  tft.setCursor(150, 0);
  if (!rtc.begin()) {
    tft.setTextColor(ILI9341_RED);
    tft.print("FAILED");
    success = false;
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.print("PASSED");
  }

  // SEESAW
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 10);
  tft.print("Seesaw ");
  tft.setCursor(150, 10);
  if (!ss.begin(0x36)) {
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILED");
    success = false;
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("PASSED");
    ss.pinMode(SEESAW_CENTER_BUTTON_PIN, INPUT_PULLUP);
    ss.pinMode(SEESAW_UP_BUTTON_PIN, INPUT_PULLUP);
    ss.pinMode(SEESAW_DOWN_BUTTON_PIN, INPUT_PULLUP);
    ss.pinMode(SEESAW_LEFT_BUTTON_PIN, INPUT_PULLUP);
    ss.pinMode(SEESAW_RIGHT_BUTTON_PIN, INPUT_PULLUP);
    ss.show();
  }

  // LSM303
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 20);
  tft.print("LSM303 ");
  tft.setCursor(150, 20);
  if (!accel.begin() || !mag.begin()) {
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILED");
    success = false;
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("PASSED");
  }

  // BME680
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 30);
  tft.print("BME680 ");
  tft.setCursor(150, 30);
  if (!bme.begin()) {
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILED");
    success = false;
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("PASSED");
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms
  }

  // QSPI FLASH
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 40);
  tft.print("QSPI Flash ");
  tft.setCursor(150, 40);
  if (!flash.begin()){
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILED");
    fail();
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("PASSED");
  }

  // SD CARD
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 50);
  tft.print("SD Card ");
  tft.setCursor(150, 50);
  if (!SD.begin(SD_CS)) {
    tft.setTextColor(ILI9341_YELLOW);
    tft.println("NOT PRESENT");
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("PRESENT");
  }

  // WiFi Module

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 60);
  tft.print("WiFi ");
  tft.setCursor(150, 60);
  WiFi.status();
  delay(100);
  if (WiFi.status() == WL_NO_MODULE) {
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILED");
    success = false;
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("PASSED");
  }

  // PyPortal Temperature sensor
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 70);
  tft.print("ADT7410 ");
  tft.setCursor(150, 70);
  if (!tempsensor.begin()) {
    tft.setTextColor(ILI9341_RED);
    tft.println("FAILED");
    success = false;
  } else {
    tft.setTextColor(ILI9341_GREEN);
    tft.println("PASSED");
  }

  if (!success) {
    fail();
  }

  analogWriteResolution(12);
  analogWrite(A0, 128);
  pinMode(SPKR_SHUTDOWN, OUTPUT);
  digitalWrite(SPKR_SHUTDOWN, LOW);

  button_pin_mask =
    (1 << SEESAW_CENTER_BUTTON_PIN) |
    (1 << SEESAW_UP_BUTTON_PIN) |
    (1 << SEESAW_DOWN_BUTTON_PIN) |
    (1 << SEESAW_LEFT_BUTTON_PIN) |
    (1 << SEESAW_RIGHT_BUTTON_PIN);


  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 100);
  tft.print("Light:");
  tft.setCursor(0, 116);
  tft.print("Temp:");
  tft.setCursor(0, 132);
  tft.print("Accel:");
  tft.setCursor(0, 148);
  tft.print("Mag:");
  tft.setCursor(0, 164);
  tft.print("BME:");
  tft.setCursor(0, 196);
  tft.print("Buttons:");

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

// Scheduling times
unsigned long now = 0;
unsigned long rtc_update_time = 0;
unsigned long accel_update_time = 0;
unsigned long mag_update_time = 0;
unsigned long bme_update_time = 0;
unsigned long altitude_update_time = 0;
unsigned long light_update_time = 0;
unsigned long temp_update_time = 0;
unsigned long pixel_update_time = 0;

int current_pixel = 4;
int pixel_direction = 1;

float altitude = 0.0;

void loop() {
  ss.digitalWrite(15, false);
  now = millis();
  tft.setTextColor(ILI9341_WHITE);

  // RTC
  if (now >= rtc_update_time) {
    rtc_update_time = now + 1000; // every second
    DateTime t = rtc.now();
    tft.fillRect(0, 84, 240, 16, ILI9341_BLACK);
    tft.setCursor(0, 84);
    printf("%4d/%02d/%02d (%s) %2d:%02d:%02d", t.year(), t.month(), t.day(), daysOfTheWeek[t.dayOfTheWeek()], t.hour(), t.minute(), t.second());
  }

  // light sensor
  if (now >= light_update_time) {
    light_update_time = now + 500;
    uint16_t light = analogRead(LIGHT_SENSOR);
    tft.fillRect(50, 100, 190, 16, ILI9341_BLACK);
    tft.setCursor(50, 100);
    printf("%3d", light);
  }

  // pyportal temp sensor
  if (now >= temp_update_time) {
    temp_update_time = now + 10000;
    float temp = tempsensor.readTempC();
    tft.fillRect(50, 116, 190, 16, ILI9341_BLACK);
    tft.setCursor(50, 116);
    printf("%5.2fC", temp);
  }

  if (now >= accel_update_time) {
    accel_update_time = now + 400;
    sensors_event_t event;
    accel.getEvent(&event);
    tft.fillRect(50, 132, 190, 16, ILI9341_BLACK);
    tft.setCursor(50, 132);
    printf("(%.2f, %.2f, %.2f) m/s^2", event.acceleration.x, event.acceleration.y, event.acceleration.z);
  }

  if (now >= mag_update_time) {
    mag_update_time = now + 600;
    sensors_event_t event;
    mag.getEvent(&event);
    tft.fillRect(50, 148, 190, 16, ILI9341_BLACK);
    tft.setCursor(50, 148);
    printf("(%.2f, %.2f, %.2f) uT", event.magnetic.x, event.magnetic.y, event.magnetic.z);
  }

  if (now >= bme_update_time) {
    bme_update_time = now + 60000;
    if (bme.performReading()) {
      tft.fillRect(50, 164, 190, 32, ILI9341_BLACK);
      tft.setCursor(50, 164);
      printf("%.2fC, %.2f hPa, %.2f%%", bme.temperature, bme.pressure / 100.0, bme.humidity);
      tft.setCursor(50, 174);
      printf("%.2f kOhms, ~%.2f m", bme.gas_resistance / 1000.0, altitude);
    }
  }

  if (now >= altitude_update_time) {
    altitude_update_time = now + 600000;
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  }

  uint32_t buttons = ss.digitalReadBulk(button_pin_mask);
  tft.fillRect(50, 196, 190, 16, ILI9341_BLACK);
  tft.setCursor(50, 196);
  printf("%s %s %s %s %s",
         (buttons & (1 << SEESAW_CENTER_BUTTON_PIN)) ? " " : "C",
         (buttons & (1 << SEESAW_UP_BUTTON_PIN)) ? " " : "U",
         (buttons & (1 << SEESAW_DOWN_BUTTON_PIN)) ? " " : "D",
         (buttons & (1 << SEESAW_LEFT_BUTTON_PIN)) ? " " : "L",
         (buttons & (1 << SEESAW_RIGHT_BUTTON_PIN)) ? " " : "R");


  uint32_t val = abs(ss.getEncoderPosition());
  val = constrain(val * 2, 0, 255);
  if (now >= pixel_update_time) {
    pixel_update_time = now + 100;

    if (current_pixel == 0 || current_pixel == 7) {
      pixel_direction *= -1;
    }
    pixels[current_pixel].show(0x000000);

    current_pixel += pixel_direction;
  }
  pixels[current_pixel].show(wheel(val)); //write to the led
}
