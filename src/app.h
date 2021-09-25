// -*- mode: c++ -*-

// Abstract application code
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

#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>
#include <lvgl.h>

// class App;

// typedef void (App::*handler_t)(lv_obj_t * obj, lv_event_t event);


class App
{
public:
  App(const char *name);
  ~App();
  virtual void activate();
  virtual void deactivate();
  virtual void encoder_changed(int32_t pos);
  virtual void nav_button_pressed(uint8_t button);
  virtual void nav_button_released(uint8_t button);
  virtual void update();
  const char *name() { return _name; }
  virtual void handle(lv_obj_t * obj, lv_event_t event) {}
  //  virtual handler_t *handler();
protected:
  static void close_event_handler(lv_obj_t * obj, lv_event_t event);
  lv_obj_t *_window;
  const char *_name;
};

#endif
