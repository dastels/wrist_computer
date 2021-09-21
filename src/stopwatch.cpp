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

#include "defines.h"
#include "globals.h"
#include "stopwatch.h"

extern lv_font_t dseg7;

extern uint8_t stopwatch_start_haptic;
extern uint8_t stopwatch_stop_haptic;

#define BASIC_MODE     (0)
#define ANIMATOR_MODE (1)
const char *mode_names[] = {"Basic", "Animator"};
const uint8_t number_of_mode_values = 2;

const uint8_t fps_values[] = {12, 18, 24, 32};
const uint8_t number_of_fps_values = 4;

char strbuf[64];

Stopwatch::Stopwatch()
  : App("Stopwatch")
  , _running(false)
{
  _mode = eeprom.get_stopwatch_mode();
  _fps = eeprom.get_frames_per_second();

  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);
  lv_obj_set_hidden(_window, true);

  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, 1);
  lv_win_set_title(_window, "Stopwatch");
  lv_obj_t *close_button = lv_win_add_btn_right(_window, LV_SYMBOL_CLOSE);
  lv_obj_set_event_cb(close_button, lv_win_close_event_cb);

  _mode_label = lv_label_create(_window, NULL);
  lv_obj_set_size(_mode_label, 150, 16);
  lv_obj_set_pos(_mode_label, 10, 21);

  _fps_label = lv_label_create(_window, NULL);
  lv_obj_set_size(_fps_label, 50, 16);
  lv_obj_set_pos(_fps_label, 160, 21);
  lv_label_set_text(_fps_label, "");

  _time_label = lv_label_create(_window, NULL);
  lv_obj_set_size(_time_label, 200, 100);
  lv_obj_align(_time_label, _window, LV_ALIGN_CENTER, -100, 0);
  lv_label_set_text(_time_label, "");
  static lv_style_t timer_style;
  lv_style_set_text_font(&timer_style, LV_STATE_DEFAULT, &dseg7);
  lv_obj_add_style(_time_label, LV_LABEL_PART_MAIN, &timer_style);

  set_mode(_mode);
  if (_mode == ANIMATOR_MODE) {
    set_fps(_fps);
    _millis_per_frame = 1000.0 / (float)(fps_values[_fps]);
  }

  reset();
  lv_task_handler();
}


Stopwatch::~Stopwatch()
{
  // cleanup UI
}


void Stopwatch::activate()
{
  lv_obj_set_hidden(_window, false);
}


void Stopwatch::deactivate()
{
  lv_obj_set_hidden(_window, true);
}


void Stopwatch::set_mode(uint8_t mode) {
  mode = constrain(mode, 0, number_of_mode_values - 1);

  if (mode != _mode) {
    eeprom.set_stopwatch_mode(mode);
    _mode = mode;
  }
  sprintf(strbuf, "%s Mode", mode_names[_mode]);
  lv_label_set_text(_mode_label, strbuf);
  if (_mode == BASIC_MODE) {
    lv_label_set_text(_fps_label, "");
  } else {
    show_fps();
  }
  reset();
}


void Stopwatch::show_fps()
{
  sprintf(strbuf, "%02d fps", fps_values[_fps]);
  lv_label_set_text(_fps_label, strbuf);
}


void Stopwatch::set_fps(uint8_t fps) {
  fps = constrain(fps, 0, number_of_fps_values - 1);
  if (fps != _fps) {
    eeprom.set_frames_per_second(fps);
    _fps = fps;
    _millis_per_frame = 1000 / (float)(fps_values[_fps]);
  }
  show_fps();
  reset();
}


void Stopwatch::update_display()
{
  if (_mode == BASIC_MODE) {
    sprintf(strbuf, "%02d:%02d:%02d.%02d", _hours, _minutes, _seconds, _hundredths);
  } else {
    sprintf(strbuf, "%02d:%02d:%02d  %02d", _hours, _minutes, _seconds, _frames);
  }
  lv_label_set_text(_time_label, strbuf);
}


void Stopwatch::start()
{
  _running = true;
  haptic.play(stopwatch_start_haptic);
  _start_time = millis();
}


void Stopwatch::stop()
{
  _running = false;
  haptic.play(stopwatch_stop_haptic);
}


void Stopwatch::reset()
{
  _hours = 0;
  _minutes = 0;
  _seconds = 0;
  _hundredths = 0;
  _frames = 0;
  _start_time = 0;
  update_display();
}


void Stopwatch::update()
{
  if (_center_pressed_time && (millis() - _center_pressed_time) > 1000) {
    reset();
    return;
  }
  if (!_running) {
    return;
  }

  uint32_t interval = millis() - _start_time;
  uint32_t subseconds = interval % 1000;
  _hundredths = subseconds / 10;
  _frames = (uint8_t)((float)subseconds / _millis_per_frame);
  interval /= 1000;
  _seconds = interval % 60;
  interval /= 60;
  _minutes = interval % 60;
  interval /= 60;
  _hours = interval & 0xFF;
  update_display();
}

void Stopwatch::nav_button_pressed(uint8_t button)
{
  logger->debug("Pressed %d", button);
  switch (button) {
  case CENTER_BUTTON_PIN:
    if (!_running) {
      _center_pressed_time = millis();
    }
    break;
  case LEFT_BUTTON_PIN:
    if (!_running && _mode > 0) {
      set_mode(_mode - 1);
    }
    break;
  case RIGHT_BUTTON_PIN:
    if (!_running && _mode < number_of_mode_values) {
      set_mode(_mode + 1);
    }
    break;
  case DOWN_BUTTON_PIN:
    if (!_running && _mode == ANIMATOR_MODE) {
      if (_fps > 0) {
        set_fps(_fps - 1);
      }
    }
    break;
  case UP_BUTTON_PIN:
    if (!_running && _mode == ANIMATOR_MODE) {
      if (_fps < number_of_fps_values) {
        set_fps(_fps + 1);
      }
    }
    break;
  }
}


void Stopwatch::nav_button_released(uint8_t button)
{
  logger->debug("Released %d", button);
  switch (button) {
  case CENTER_BUTTON_PIN:
    if (!_center_pressed_time || (millis() - _center_pressed_time) < 1000) {
      if (_running) {
        stop();
      } else {
        start();
      }
    }
    _center_pressed_time = 0;
    break;
  default:
    break;
  }
}
