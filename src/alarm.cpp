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
  : App("Alarm", true)
  , _running(false)
{
  alarm_instance = this;

  char options[192];
  options[0] = 0;
  char option_buf[8];

  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);
  lv_obj_set_hidden(_window, true);

  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, 1);
  lv_win_set_title(_window, "Alarm");
  lv_obj_t *close_button = lv_win_add_btn_right(_window, LV_SYMBOL_CLOSE);
  lv_obj_set_event_cb(close_button, App::close_event_handler);

  _hour_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_hour_roller, _window, LV_ALIGN_IN_LEFT_MID, 10, 0);
  lv_roller_set_auto_fit(_hour_roller, true);
  for (int i = 0; i < 24; i++) {
    sprintf(option_buf, (i ? "\n%02d" : "00"), i);
    strcat(options, option_buf);
  }
  lv_roller_set_options(_hour_roller, options, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_hour_roller, 4);
  _hours = 0;

  lv_obj_t *colon_label = lv_label_create(_window, NULL);
  lv_obj_align(colon_label, _hour_roller, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  lv_label_set_text(colon_label, ":");

  _minute_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_minute_roller, _window, LV_ALIGN_IN_LEFT_MID, 70, 0);
  lv_roller_set_auto_fit(_minute_roller, true);
  options[0] = 0;
  for (int i = 0; i < 60; i++) {
    sprintf(option_buf, (i ? "\n%02d" : "00"), i);
    strcat(options, option_buf);
  }
  lv_roller_set_options(_minute_roller, options, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_minute_roller, 4);
  _minutes = 0;

  _running_indicator = lv_label_create(_window, NULL);
  lv_obj_align(_running_indicator, _window, LV_ALIGN_IN_RIGHT_MID, -50, -50);
  lv_label_set_text(_running_indicator, "");

  _ringing_indicator = lv_label_create(_window, NULL);
  lv_obj_align(_ringing_indicator, _window, LV_ALIGN_IN_RIGHT_MID, -50, 50);
  lv_label_set_text(_ringing_indicator, "");

}


Alarm::~Alarm()
{
}


void Alarm::activate()
{
  App::activate();
  if (_running) {
    lv_obj_clear_state(_minute_roller, LV_STATE_FOCUSED);
    lv_obj_clear_state(_hour_roller, LV_STATE_FOCUSED);
  } else {
    lv_obj_clear_state(_minute_roller, LV_STATE_FOCUSED);
    lv_obj_add_state(_hour_roller, LV_STATE_FOCUSED);
    _focussed_roller = _hour_roller;
  }
  update_display();
}


void Alarm::deactivate()
{
  App::deactivate();
}


void Alarm::update_display()
{
  if (lv_obj_is_visible(_window)) {
    lv_label_set_text(_running_indicator, _running ? "RUNNING" : "");
    lv_label_set_text(_ringing_indicator, _ringing ? "RINGING" : "");
    lv_task_handler();
  }
}


void Alarm::update_setting()
{
  _hours = lv_roller_get_selected(_hour_roller);
  _minutes = lv_roller_get_selected(_minute_roller);
}


void Alarm::start()
{
  lv_obj_clear_state(_minute_roller, LV_STATE_FOCUSED);
  lv_obj_clear_state(_hour_roller, LV_STATE_FOCUSED);
  _running = true;
  _ringing = false;
  update_display();
}


void Alarm::stop()
{
  lv_obj_clear_state(_minute_roller, LV_STATE_FOCUSED);
  lv_obj_add_state(_hour_roller, LV_STATE_FOCUSED);
  _focussed_roller = _hour_roller;

  _running = false;
  _ringing = false;
  update_display();
}


void Alarm::ring(unsigned long now)
{
  if (!_ringing) {
    return;
  }

  if (!_sound_on && now >= _sound_on_time) {
    _sound_off_time = now + NOISY_TIME;
    _sound_on = true;
    tone(A0, 1000);
    // haptic
  } else if (_sound_on && now >= _sound_off_time) {
    _sound_on = false;
    _sound_on_time = now + QUIET_TIME;
    noTone(A0);
  }
}


void Alarm::start_ringing(unsigned long now)
{
  activate();                   // make the alarm app visible
  disable_sensor_tasks();       // so they don't mess up the timing
  _ringing = true;
  if (!silent) {
    digitalWrite(SPKR_ENABLE, HIGH);
  }
  _sound_off_time = now + QUIET_TIME;
  _sound_on = true;
  tone(A0, 1000);
}


void Alarm::stop_ringing()
{
  digitalWrite(SPKR_ENABLE, LOW);
  noTone(A0);
  _ringing = false;
  enable_sensor_tasks();
  update_display();
}


void Alarm::update(unsigned long now)
{
  if (!_running) return;

  if (_ringing) {
    ring(now);
  } else {
    DateTime t = rtc.now();
    if (t.hour() == _hours && t.minute() == _minutes) {
      if (!_triggered) {
        _triggered = true;
        start_ringing(now);
      }
    } else if (_triggered) {
      stop_ringing();
      _triggered = false;
    }
  }
  update_display();
}


void Alarm::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;
  uint16_t selected;
  uint16_t count;

  logger->debug("Alarm::event_handler");
  logger->debug("Event: %d", event);

  switch (event) {
  case LV_EVENT_CLICKED:
    if (alarm_instance->_ringing) {
      alarm_instance->stop_ringing();
    } else {
      if (alarm_instance->_running) {
        alarm_instance->stop();
      } else {
        alarm_instance->start();
      }
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
      if (!alarm_instance->_running) {
        logger->debug("LEFT");
        if (alarm_instance->_focussed_roller == alarm_instance->_minute_roller) {
          lv_obj_clear_state(alarm_instance->_focussed_roller, LV_STATE_FOCUSED);
          alarm_instance->_focussed_roller = alarm_instance->_hour_roller;
          lv_obj_add_state(alarm_instance->_focussed_roller, LV_STATE_FOCUSED);
        }
      }
      break;
    case LV_KEY_RIGHT:
      if (!alarm_instance->_running) {
        logger->debug("RIGHT");
        if (alarm_instance->_focussed_roller == alarm_instance->_hour_roller) {
          lv_obj_clear_state(alarm_instance->_focussed_roller, LV_STATE_FOCUSED);
          alarm_instance->_focussed_roller = alarm_instance->_minute_roller;
          lv_obj_add_state(alarm_instance->_focussed_roller, LV_STATE_FOCUSED);
        }
      }
      break;
    case LV_KEY_PREV:
    case LV_KEY_UP:
      if (!alarm_instance->_running) {
        selected = lv_roller_get_selected(alarm_instance->_focussed_roller);
        if (selected > 0) {
          lv_roller_set_selected(alarm_instance->_focussed_roller, selected - 1, LV_ANIM_ON);
          alarm_instance->update_setting();
        }
      }
      break;
    case LV_KEY_NEXT:
    case LV_KEY_DOWN:
      if (!alarm_instance->_running) {
        selected = lv_roller_get_selected(alarm_instance->_focussed_roller);
        count = lv_roller_get_option_cnt(alarm_instance->_focussed_roller);
        if (selected < count) {
          lv_roller_set_selected(alarm_instance->_focussed_roller, selected + 1, LV_ANIM_ON);
          alarm_instance->update_setting();
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
