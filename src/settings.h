// -*- mode: c++ -*-

// Settings App
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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "lvgl.h"
#include "app.h"

class Settings: public App
{
public:
  Settings();
  ~Settings();
  void update(unsigned long now);
  void activate();
  void deactivate();
  void handle(lv_obj_t * obj, lv_event_t event) { Settings::event_handler(obj, event); }

 private:
  lv_obj_t *_hour_roller;
  lv_obj_t *_minute_roller;
  lv_obj_t *_year_roller;
  lv_obj_t *_month_roller;
  lv_obj_t *_day_roller;
  lv_obj_t *_silence_switch;

  lv_obj_t *_controls[6];
  uint8_t _focussed_control;
  uint8_t _number_of_controls;

  static void set_time(lv_obj_t * obj, lv_event_t event);
  static void set_date(lv_obj_t * obj, lv_event_t event);
  static void set_silence(lv_obj_t * obj, lv_event_t event);
  static void event_handler(lv_obj_t * obj, lv_event_t event);
};

#endif
