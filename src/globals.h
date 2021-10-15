// -*- mode: c++ -*-

// Globals
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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <stdint.h>
#include <lvgl.h>
#include <RTClib.h>

#include "eeprom.h"
#include "logging.h"
#include "haptic.h"
#include "sensor_readings.h"
#include "app.h"
#include "idle_screen.h"

extern Logger *logger;
extern Haptic haptic;
extern RTC_DS3231 rtc;
extern Eeprom eeprom;
extern SensorReadings sensor_readings;
extern IdleScreen *idle;
extern App *current_app;
extern lv_indev_t *encoder_dev;
extern lv_indev_t *navpad_dev;
extern bool silent;
extern char strbuf[];

void disable_sensor_tasks();
void enable_sensor_tasks();
bool is_current_app(App *app);

#endif
