// -*- mode: c++ -*-

// Application menu
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

#ifndef __APP_MENU_H__
#define __APP_MENU_H__

#include "app.h"
#include "lvgl.h"

class IdleScreen;

class AppMenu
{
  friend IdleScreen;
 public:
  AppMenu(lv_obj_t *window);
  bool register_app(App *app);
  void choose_app();

 private:
  void open();
  void close();
  static void event_handler(lv_obj_t * obj, lv_event_t event);
  App *_apps[16];
  uint8_t _number_of_apps;
  lv_obj_t *_window;
  lv_obj_t *_app_roller;

  friend IdleScreen;
};

#endif
