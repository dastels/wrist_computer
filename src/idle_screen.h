// -*- mode: c++ -*-

// Idle/background screen/app
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

#ifndef __IDLE_SCREEN_H__
#define __IDLE_SCREEN_H__

#include <lvgl.h>

#include "app.h"
#include "app_menu.h"

class IdleScreen: public App
{
 public:
  IdleScreen();
  void activate();
  void deactivate();
  void update();
  /* void encoder_changed(int32_t pos); */
  /* void nav_button_pressed(uint8_t button); */
  /* void nav_button_released(uint8_t button); */
  bool register_app(App *app);
  void choosing_app(bool on_off) { _choosing_app = on_off; }
  bool choosing_app(void) { return _choosing_app; }
  void handle(lv_obj_t * obj, lv_event_t event) { IdleScreen::event_handler(obj, event); }
 private:
  lv_obj_t *_window;
  AppMenu *_app_menu;
  int32_t _old_encoder_pos;
  bool _choosing_app;
  lv_group_t * _group;
  static void event_handler(lv_obj_t * obj, lv_event_t event);
};

#endif
