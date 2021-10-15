// -*- mode: c++ -*-

// Alarm App
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

#ifndef __ALARM_H__
#define __ALARM_H__

#include "lvgl.h"
#include "app.h"

#define NOISY_TIME (500)        // tone length for alarm ringing
#define QUIET_TIME (750)        // quiet time between tone bursts for alarm ringing


// Alarm app
// center button starts/stops
// up/down change mode:
//   - hh:mm:ss.hh (hh = hundredths of a second)
//   - hh:mm:ss ff (ff = frames)
//     left/right change frames/second (6, 12, 24, 32, whatever) this is remembered

class Alarm: public App
{
public:
  Alarm();
  ~Alarm();
  void start();
  void stop();
  void reset();
  void update(unsigned long now);
  void activate();
  void deactivate();
  void handle(lv_obj_t * obj, lv_event_t event) { Alarm::event_handler(obj, event); }

private:
  void update_display();
  void update_setting();
  void ring(unsigned long now);
  void start_ringing(unsigned long now);
  void stop_ringing();

  lv_obj_t *_hour_roller;
  lv_obj_t *_minute_roller;
  lv_obj_t *_focussed_roller;
  lv_obj_t *_time_label;
  lv_obj_t *_running_indicator;
  lv_obj_t *_ringing_indicator;

  // operastional states
  bool _running;                // Is the alarm watching the time
  bool _ringing;                // Is the alarm ringing
  bool _triggered;              // Has the alarm been triggered

  // manage the ringing
  unsigned long _sound_on_time;  // time the tone should turn on
  unsigned long _sound_off_time; // time the tone should turn off
  bool _sound_on;                // whether the tone is on

  uint8_t _hours;
  uint8_t _minutes;

  static void event_handler(lv_obj_t * obj, lv_event_t event);
};

#endif
