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

#include "defines.h"
#include "globals.h"
#include "alarm.h"

extern lv_font_t dseg7;

Alarm *alarm_instance;

Alarm::Alarm()
  : App("Alarm")
  , _running(false)
{
  alarm_instance = this;

  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);
  lv_group_t * _group = lv_group_create();
  lv_group_add_obj(_group, _window);

  lv_obj_set_hidden(_window, true);

  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, 1);
  lv_win_set_title(_window, "Alarm");
  lv_obj_t *close_button = lv_win_add_btn_right(_window, LV_SYMBOL_CLOSE);
  lv_obj_set_event_cb(close_button, App::close_event_handler);

}


Alarm::~Alarm()
{
}


void Alarm::activate()
{
  App::activate();
}


void Alarm::deactivate()
{
  App::deactivate();
}




void Alarm::update_display()
{
}


void Alarm::start()
{
  _running = true;
  _start_time = millis();
}


void Alarm::stop()
{
  _running = false;
}


void Alarm::reset()
{
  _hours = 0;
  _minutes = 0;
  _seconds = 0;
  _hundredths = 0;
  _frames = 0;
  _start_time = 0;
  update_display();
}


void Alarm::update()
{
}


void Alarm::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;
  logger->debug("Alarm::event_handler");
  logger->debug("Event: %d", event);

  switch (event) {
  case LV_EVENT_CLICKED:
    break;
  case LV_EVENT_KEY:
    key_ptr = (uint32_t*)(lv_event_get_data());
    logger->debug("Key event: %d", *key_ptr);
    switch (*key_ptr) {
    case LV_KEY_ESC:
      App::close_event_handler(nullptr, 0);
      break;
    case LV_KEY_LEFT:
      logger->debug("LEFT");
      break;
    case LV_KEY_RIGHT:
      logger->debug("RIGHT");
      break;
    case LV_KEY_DOWN:
      logger->debug("DOWN");
      break;
    case LV_KEY_UP:
      logger->debug("UP");
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}
