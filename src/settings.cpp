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

#include "defines.h"
#include "globals.h"
#include "settings.h"

Settings *settings_instance;

Settings::Settings()
  : App("Settings")
{
  char option_buf[8];

  _focussed_control = 0;
  uint8_t control_index = 0;

  settings_instance = this;
  DateTime now = rtc.now();

  _window = lv_win_create(lv_scr_act(), NULL);
  lv_obj_set_hidden(_window, true);

  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, 1);
  lv_win_set_title(_window, "Settings");
  lv_obj_t *close_button = lv_win_add_btn_right(_window, LV_SYMBOL_CLOSE);
  lv_obj_set_event_cb(close_button, App::close_event_handler);
  lv_win_set_scrollbar_mode(_window, LV_SCRLBAR_MODE_OFF);

  // Time ------------------------------------------------------------

  _hour_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_hour_roller, _window, LV_ALIGN_IN_LEFT_MID, 5, -40);
  lv_roller_set_auto_fit(_hour_roller, true);
  strbuf[0] = 0;
  for (int i = 0; i < 24; i++) {
    sprintf(option_buf, (i ? "\n%02d" : "00"), i);
    strcat(strbuf, option_buf);
  }
  lv_roller_set_options(_hour_roller, strbuf, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_hour_roller, 3);
  lv_obj_set_event_cb(_hour_roller, Settings::set_time);
  lv_obj_add_state(_hour_roller, LV_STATE_FOCUSED);
  lv_roller_set_selected(_hour_roller, now.hour(), LV_ANIM_OFF);

  _controls[control_index++] = _hour_roller;

  lv_obj_t *colon_label = lv_label_create(_window, NULL);
  lv_obj_align(colon_label, _hour_roller, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
  lv_label_set_text(colon_label, ":");

  _minute_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_minute_roller, _hour_roller, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_roller_set_auto_fit(_minute_roller, true);
  strbuf[0] = 0;
  for (int i = 0; i < 60; i++) {
    sprintf(option_buf, (i ? "\n%02d" : "00"), i);
    strcat(strbuf, option_buf);
  }
  lv_roller_set_options(_minute_roller, strbuf, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_minute_roller, 3);
  lv_obj_set_event_cb(_minute_roller, Settings::set_time);
  lv_roller_set_selected(_minute_roller, now.minute(), LV_ANIM_OFF);

  _controls[control_index++] = _minute_roller;

  // Date ------------------------------------------------------------
  _day_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_day_roller, _minute_roller, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
  lv_roller_set_auto_fit(_day_roller, true);
  strbuf[0] = 0;
  for (int i = 1; i < 32; i++) {
    sprintf(option_buf, (i > 1) ? "\n%02d" : "%02d", i);
    strcat(strbuf, option_buf);
  }
  lv_roller_set_options(_day_roller, strbuf, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_day_roller, 3);
  lv_obj_set_event_cb(_day_roller, Settings::set_date);
  lv_roller_set_selected(_day_roller, now.day() - 1, LV_ANIM_OFF);

  _controls[control_index++] = _day_roller;

  lv_obj_t *slash_label = lv_label_create(_window, NULL);
  lv_obj_align(slash_label, _day_roller, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
  lv_label_set_text(slash_label, "/");

  _month_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_month_roller, _day_roller, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_roller_set_auto_fit(_month_roller, true);
  lv_roller_set_options(_month_roller, "Jan\nFeb\nMar\nApr\nMay\nJun\nJul\nAug\nSep\nOct\nNov\nDec", LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_month_roller, 3);
  lv_obj_set_event_cb(_month_roller, Settings::set_date);
  lv_roller_set_selected(_month_roller, now.month() - 1, LV_ANIM_OFF);

  _controls[control_index++] = _month_roller;

  slash_label = lv_label_create(_window, NULL);
  lv_obj_align(slash_label, _month_roller, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
  lv_label_set_text(slash_label, "/");

  _year_roller = lv_roller_create(_window, NULL);
  lv_obj_align(_year_roller, _month_roller, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_roller_set_auto_fit(_year_roller, true);
  strbuf[0] = 0;
  for (int i = 2020; i < 2050; i++) {
    sprintf(option_buf, (i == 2020 ? "%4d" : "\n%4d"), i);
    strcat(strbuf, option_buf);
  }
  lv_roller_set_options(_year_roller, strbuf, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_year_roller, 3);
  lv_obj_set_event_cb(_year_roller, Settings::set_date);
  lv_roller_set_selected(_year_roller, now.year() - 2020, LV_ANIM_OFF);

  _controls[control_index++] = _year_roller;

  // // Silence ------------------------------------------------------------

  lv_obj_t *silent_label = lv_label_create(_window, NULL);
  lv_obj_align(silent_label, _window, LV_ALIGN_IN_LEFT_MID, 5, 55);
  lv_label_set_text(silent_label, "Silent");

  lv_obj_t *_silence_switch = lv_switch_create(_window, NULL);
  lv_obj_align(_silence_switch, silent_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_obj_set_event_cb(_silence_switch, Settings::set_silence);

  _controls[control_index++] = _silence_switch;

  _number_of_controls = 6;

  _focussed_control = 0;
}


Settings::~Settings()
{
}


void Settings::activate()
{
  App::activate();
}


void Settings::deactivate()
{
  App::deactivate();
}


void Settings::update(unsigned long now)
{
}


void print_datetime(DateTime *dt)
{
  dt->toString(strbuf);
  logger->debug("Datetime setting: %s", strbuf);
}


void Settings::set_time(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED) {
    logger->debug("Settings::set_time");
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(),
                        now.month(),
                        now.day(),
                        lv_roller_get_selected(settings_instance->_hour_roller),
                        lv_roller_get_selected(settings_instance->_minute_roller),
                        0));
  }
}


void Settings::set_date(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED) {
    logger->debug("Settings::set_date");
    DateTime now = rtc.now();
    rtc.adjust(DateTime(lv_roller_get_selected(settings_instance->_year_roller) + 2020,
                        lv_roller_get_selected(settings_instance->_month_roller) + 1,
                        lv_roller_get_selected(settings_instance->_day_roller) + 1,
                        now.hour(),
                        now.minute(),
                        now.second()));
  }
}


void Settings::set_silence(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED) {
   silent = lv_switch_get_state(settings_instance->_silence_switch);
  }
}


void Settings::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;
  uint16_t selected;
  uint16_t count;
  lv_obj_t *focussed_control = settings_instance->_controls[settings_instance->_focussed_control];
  lv_obj_type_t obj_type;

  logger->debug("Settings::event_handler");
  logger->debug("Event: %d", event);

  lv_obj_get_type(focussed_control, &obj_type);
  bool is_switch = strcmp(obj_type.type[0], "lv_switch") == 0;
  bool is_roller = strcmp(obj_type.type[0], "lv_roller") == 0;

  switch (event) {
  case LV_EVENT_CLICKED:        // toggle switches
    if (is_switch) {
      lv_switch_toggle(focussed_control, LV_ANIM_ON);
      lv_event_send(focussed_control, LV_EVENT_VALUE_CHANGED, NULL);
    }
    break;
  case LV_EVENT_KEY:
    key_ptr = (uint32_t*)(lv_event_get_data());
    logger->debug("Key event: %d", *key_ptr);
    switch (*key_ptr) {
    case LV_KEY_ESC:
      App::close_event_handler(nullptr, 0);
      break;
    case LV_KEY_LEFT:           // move to previous control
      logger->debug("LEFT");
      if (settings_instance->_focussed_control > 0) {
        lv_obj_clear_state(focussed_control, LV_STATE_FOCUSED);
        settings_instance->_focussed_control--;
        lv_obj_add_state(settings_instance->_controls[settings_instance->_focussed_control], LV_STATE_FOCUSED);
      }
      break;
    case LV_KEY_RIGHT:          // move to next control
      logger->debug("RIGHT");
      if (settings_instance->_focussed_control < settings_instance->_number_of_controls - 1) {
        lv_obj_clear_state(focussed_control, LV_STATE_FOCUSED);
        settings_instance->_focussed_control++;
        lv_obj_add_state(settings_instance->_controls[settings_instance->_focussed_control], LV_STATE_FOCUSED);
      }
      break;
    case LV_KEY_PREV:           // move roller to previous value
    case LV_KEY_UP:
      if (is_roller) { // a roller
        selected = lv_roller_get_selected(focussed_control);
        if (selected > 0) {
          lv_roller_set_selected(focussed_control, selected - 1, LV_ANIM_ON);
          lv_event_send(focussed_control, LV_EVENT_VALUE_CHANGED, NULL);
        }
      }
      break;
    case LV_KEY_NEXT:           // move roller to next value
    case LV_KEY_DOWN:
      if (is_roller) { // a roller
        selected = lv_roller_get_selected(focussed_control);
        count = lv_roller_get_option_cnt(focussed_control);
        if (selected < count - 1) {
          lv_roller_set_selected(focussed_control, selected + 1, LV_ANIM_ON);
          lv_event_send(focussed_control, LV_EVENT_VALUE_CHANGED, NULL);
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
