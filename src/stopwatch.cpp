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

extern lv_font_t dseg7_32;

extern uint8_t stopwatch_start_haptic;
extern uint8_t stopwatch_stop_haptic;

Stopwatch *stopwatch_instance;

#define BASIC_MODE     (0)
#define ANIMATOR_MODE (1)
const char *mode_names[] = {"Basic", "Animator"};
const uint8_t number_of_mode_values = 2;

const uint8_t fps_values[] = {12, 18, 24, 32};
const uint8_t number_of_fps_values = 4;


Stopwatch::Stopwatch()
  : App("Stopwatch")
  , _running(false)
{
  stopwatch_instance = this;

  _mode = eeprom.get_stopwatch_mode();
  _fps = eeprom.get_frames_per_second();

  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);
  // lv_obj_set_event_cb(_window, Stopwatch::event_handler);
  lv_group_t * _group = lv_group_create();
  lv_group_add_obj(_group, _window);

  lv_obj_set_hidden(_window, true);

  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, 1);
  lv_win_set_title(_window, "Stopwatch");
  lv_obj_t *close_button = lv_win_add_btn_right(_window, LV_SYMBOL_CLOSE);
  lv_obj_set_event_cb(close_button, App::close_event_handler);

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
  lv_label_set_recolor(_time_label, true);
  static lv_style_t timer_style;
  lv_style_set_text_font(&timer_style, LV_STATE_DEFAULT, &dseg7_32);
  lv_obj_add_style(_time_label, LV_LABEL_PART_MAIN, &timer_style);

  set_mode(_mode);
  if (_mode == ANIMATOR_MODE) {
    set_fps(_fps);
    _millis_per_frame = 1000.0 / (float)(fps_values[_fps]);
  }

  reset();
}


Stopwatch::~Stopwatch()
{
  // cleanup UI
}


void Stopwatch::activate()
{
  App::activate();
  reset();
}


void Stopwatch::deactivate()
{
  stop();
  App::deactivate();
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
    sprintf(strbuf, "#%s %02d:%02d:%02d.%02d#", (_running ? "00FF00" : "888888"), _hours, _minutes, _seconds, _hundredths);
  } else {
    sprintf(strbuf, "#%s %02d:%02d:%02d  %02d#", (_running ? "00FF00" : "888888"), _hours, _minutes, _seconds, _frames);
  }
  lv_label_set_text(_time_label, strbuf);
}


void Stopwatch::start()
{
  disable_sensor_tasks();
  _running = true;
  update_display();
  haptic.play(stopwatch_start_haptic);
  _start_time = millis();
}


void Stopwatch::stop()
{
  _running = false;
  update_display();
  haptic.play(stopwatch_stop_haptic);
  enable_sensor_tasks();
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


void Stopwatch::update(unsigned long now)
{
  if (_running) {
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
}


void Stopwatch::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;
  logger->debug("StopWatch::event_handler");
  logger->debug("Event: %d", event);

  switch (event) {
  case LV_EVENT_CLICKED:
    if (stopwatch_instance->_running) {
      stopwatch_instance->stop();
    } else {
      stopwatch_instance->start();
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
      logger->debug("LEFT");
      if (!stopwatch_instance->_running && stopwatch_instance->_mode > 0) {
        stopwatch_instance->set_mode(stopwatch_instance->_mode - 1);
      }
      break;
    case LV_KEY_RIGHT:
      logger->debug("RIGHT");
      if (!stopwatch_instance->_running && stopwatch_instance->_mode < number_of_mode_values) {
        stopwatch_instance->set_mode(stopwatch_instance->_mode + 1);
      }
      break;
    case LV_KEY_DOWN:
      logger->debug("DOWN");
      if (!stopwatch_instance->_running && stopwatch_instance->_mode == ANIMATOR_MODE) {
        if (stopwatch_instance->_fps > 0) {
          stopwatch_instance->set_fps(stopwatch_instance->_fps - 1);
        }
      }
      break;
    case LV_KEY_UP:
      logger->debug("UP");
      if (!stopwatch_instance->_running && stopwatch_instance->_mode == ANIMATOR_MODE) {
        if (stopwatch_instance->_fps < number_of_fps_values) {
          stopwatch_instance->set_fps(stopwatch_instance->_fps + 1);
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
