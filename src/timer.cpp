// -*- mode: c++ -*-

// Timer App
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
#include "timer.h"

extern lv_font_t dseg7_32;
extern uint8_t timer_ring_haptic;

Timer *timer_instance;


Timer::Timer()
  : App("Timer")
  , _running(false)
{
  timer_instance = this;

  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);

  lv_obj_set_hidden(_window, true);

  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, LV_TXT_FLAG_CENTER);
  lv_win_set_title(_window, "");
  lv_win_set_scrollbar_mode(_window, LV_SCRLBAR_MODE_OFF);
  static lv_style_t window_style;
  lv_style_set_bg_color(&window_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_add_style(_window, LV_WIN_PART_BG, &window_style);

  _hour_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_hour_roller, _window, LV_ALIGN_IN_LEFT_MID, 10, 0);
  lv_roller_set_auto_fit(_hour_roller, true);
  lv_roller_set_options(_hour_roller, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10", LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_hour_roller, 4);
  lv_obj_set_event_cb(_hour_roller, Timer::event_handler);

  lv_obj_t *colon_label = lv_label_create(_window, NULL);
  lv_obj_align(colon_label, _hour_roller, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  lv_label_set_text(colon_label, ":");

  _minute_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_minute_roller, _window, LV_ALIGN_IN_LEFT_MID, 70, 0);
  lv_roller_set_auto_fit(_minute_roller, true);
  char options[192];
  options[0] = 0;
  char option_buf[8];
  for (int i = 0; i < 60; i++) {
    sprintf(option_buf, (i ? "\n%02d" : "00"), i);
    strcat(options, option_buf);
  }
  lv_roller_set_options(_minute_roller, options, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_minute_roller, 4);
  lv_obj_set_event_cb(_minute_roller, Timer::event_handler);

  _focussed_roller = _hour_roller;
  lv_obj_add_state(_hour_roller, LV_STATE_FOCUSED);

  _time_label = lv_label_create(_window, NULL);
  lv_obj_align(_time_label, _window, LV_ALIGN_IN_RIGHT_MID, -160, 0);
  lv_label_set_text(_time_label, "");
  static lv_style_t timer_style;
  lv_label_set_recolor(_time_label, true);
  lv_style_set_text_font(&timer_style, LV_STATE_DEFAULT, &dseg7_32);
  lv_obj_add_style(_time_label, LV_LABEL_PART_MAIN, &timer_style);

  update_setting();
}


Timer::~Timer()
{
}


void Timer::activate()
{
  App::activate();
}


void Timer::deactivate()
{
  stop();
  App::deactivate();
}


void Timer::update_display()
{
  sprintf(strbuf, "#%s %02d:%02d:%02d#", (_running ? "00FF00" : "888888"), _hours, _minutes, _seconds);
  lv_label_set_text(_time_label, strbuf);
}


void Timer::update_setting()
{
  _hours = lv_roller_get_selected(_hour_roller);
  _minutes = lv_roller_get_selected(_minute_roller);
  _seconds = 0;
  update_display();
}


void Timer::start()
{
  disable_sensor_tasks();
  update_setting();
  _running = true;
  _seconds_countdown = _hours * 3600 + _minutes * 60;
  _start_time = millis();
  lv_obj_clear_state(_minute_roller, LV_STATE_FOCUSED);
  lv_obj_clear_state(_hour_roller, LV_STATE_FOCUSED);
  lv_obj_set_state(_minute_roller, LV_STATE_DISABLED);
  lv_obj_set_state(_hour_roller, LV_STATE_DISABLED);

  update_display();
}


void Timer::stop()
{
  enable_sensor_tasks();
  _running = false;
  lv_obj_clear_state(_minute_roller, LV_STATE_DISABLED);
  lv_obj_clear_state(_hour_roller, LV_STATE_DISABLED);
  lv_obj_clear_state(_minute_roller, LV_STATE_FOCUSED);
  lv_obj_add_state(_hour_roller, LV_STATE_FOCUSED);
  _focussed_roller = _hour_roller;

  update_display();
}


void Timer::reset()
{
  _hours = 0;
  _minutes = 0;
  _seconds = 0;
  _start_time = 0;
  update_display();
}


void Timer::update(unsigned long now)
{
  if (_running) {
    uint32_t elapsed = (now - _start_time) / 1000;
    uint32_t remaining = _seconds_countdown - elapsed;

    _seconds = remaining % 60;
    remaining /= 60;
    _minutes = remaining % 60;
    remaining /= 60;
    _hours = remaining & 0xFF;
    update_display();
    if (_hours == 0 && _minutes == 0 && _seconds == 0) {
      stop();
      ring();
    }
  }
}


void Timer::ring()
{
  haptic.play(timer_ring_haptic);

  if (!silent) {
    digitalWrite(SPKR_ENABLE, HIGH);
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 3; j++) {
        tone(A0, 2000, 250);
        delay(400);
      }
      delay(500);
    }
    digitalWrite(SPKR_ENABLE, LOW);
  }
}


void Timer::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;
  uint16_t selected;
  uint16_t count;

  logger->debug("Timer::event_handler");
  logger->debug("Event: %d", event);

  switch (event) {
  case LV_EVENT_CLICKED:
    if (timer_instance->_running) {
      timer_instance->stop();
    } else {
      timer_instance->start();
    }
    break;
  case LV_EVENT_KEY:
    key_ptr = (uint32_t*)(lv_event_get_data());
    logger->debug("Key event: %d", *key_ptr);
    switch (*key_ptr) {
    case LV_KEY_ESC:
      App::close_event_handler(nullptr, 0);
      break;
    case LV_KEY_LEFT:
      if (!timer_instance->_running) {
        logger->debug("LEFT");
        if (timer_instance->_focussed_roller == timer_instance->_minute_roller) {
          lv_obj_clear_state(timer_instance->_focussed_roller, LV_STATE_FOCUSED);
          timer_instance->_focussed_roller = timer_instance->_hour_roller;
          lv_obj_add_state(timer_instance->_focussed_roller, LV_STATE_FOCUSED);
        }
      }
      break;
    case LV_KEY_RIGHT:
      if (!timer_instance->_running) {
        logger->debug("RIGHT");
        if (timer_instance->_focussed_roller == timer_instance->_hour_roller) {
          lv_obj_clear_state(timer_instance->_focussed_roller, LV_STATE_FOCUSED);
          timer_instance->_focussed_roller = timer_instance->_minute_roller;
          lv_obj_add_state(timer_instance->_focussed_roller, LV_STATE_FOCUSED);
        }
      }
      break;
    case LV_KEY_PREV:
    case LV_KEY_UP:
      if (!timer_instance->_running) {
        selected = lv_roller_get_selected(timer_instance->_focussed_roller);
        if (selected > 0) {
          lv_roller_set_selected(timer_instance->_focussed_roller, selected - 1, LV_ANIM_ON);
          timer_instance->update_setting();
        }
      }
      break;
    case LV_KEY_NEXT:
    case LV_KEY_DOWN:
      if (!timer_instance->_running) {
        selected = lv_roller_get_selected(timer_instance->_focussed_roller);
        count = lv_roller_get_option_cnt(timer_instance->_focussed_roller);
        if (selected < count) {
          lv_roller_set_selected(timer_instance->_focussed_roller, selected + 1, LV_ANIM_ON);
          timer_instance->update_setting();
        }
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}
