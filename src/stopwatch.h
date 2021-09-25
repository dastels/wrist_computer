// -*- mode: c++ -*-

// Stopwatch App
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

#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

#include "lvgl.h"
#include "app.h"

// Stopwatch app
// center button starts/stops
// up/down change mode:
//   - hh:mm:ss.hh (hh = hundredths of a second)
//   - hh:mm:ss ff (ff = frames)
//     left/right change frames/second (6, 12, 24, 32, whatever) this is remembered

class Stopwatch: public App
{
public:
  Stopwatch();
  ~Stopwatch();
  void start();
  void stop();
  void reset();
  void update();
  void activate();
  void deactivate();
  // handler() { return Stopwatch::event-handler; }
  // void nav_button_pressed(uint8_t button);
  // void nav_button_released(uint8_t button);
  void handle(lv_obj_t * obj, lv_event_t event) { Stopwatch::event_handler(obj, event); }

private:
  void set_mode(uint8_t mode);
  void set_fps(uint8_t fps);
  void show_fps();
  void update_display();
  uint8_t _mode;
  uint8_t _fps;
  float _millis_per_frame;

  lv_obj_t *_mode_label;
  lv_obj_t *_fps_label;
  lv_obj_t *_time_label;

  bool _running;
  uint32_t _start_time;
  uint8_t _hours;
  uint8_t _minutes;
  uint8_t _seconds;
  uint8_t _hundredths;
  uint8_t _frames;
  lv_group_t * _group;

  static void event_handler(lv_obj_t * obj, lv_event_t event);
};

#endif
