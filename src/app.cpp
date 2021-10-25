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

#include <WiFiNINA.h>

#include "defines.h"
#include "globals.h"
#include "app.h"


App::App(const char *name, bool make_persistant)
  :_name(name)
  , _persistant(make_persistant)
  , _battery_icon(nullptr)
{
}

App::~App()
{
}

void App::activate()
{
  logger->debug("Activating %s", _name);
  lv_obj_set_hidden(_window, false);
  lv_obj_add_state(_window, LV_STATE_FOCUSED);
}


void App::deactivate()
{
  logger->debug("Deactivating %s", _name);
  lv_obj_set_hidden(_window, true);
}


void App::update_time_display(DateTime *now)
{
  if (_battery_icon == nullptr) {
    lv_win_set_btn_width(_window, 25);

    logger->debug("Initializing titlebar indicators");
    lv_obj_t *battery_button = lv_win_add_btn_right(_window, LV_SYMBOL_BATTERY_FULL);
    _battery_icon = (lv_obj_get_child(battery_button, NULL));

    lv_obj_t *sd_button = lv_win_add_btn_left(_window, LV_SYMBOL_SD_CARD);
    _sd_icon = (lv_obj_get_child(sd_button, NULL));

    lv_obj_t *wifi_button = lv_win_add_btn_left(_window, LV_SYMBOL_WIFI);
    _wifi_icon = (lv_obj_get_child(wifi_button, NULL));

  }

  sprintf(strbuf, "%4d/%02d/%02d %02d:%02d:%02d", now->year(), now->month(), now->day(), now->hour(), now->minute(), now->second());
  lv_win_set_title(_window, strbuf);

  if (sensor_readings.battery_percentage > 95.0) {
    lv_img_set_src(_battery_icon, LV_SYMBOL_BATTERY_FULL);
  } else if (sensor_readings.battery_percentage > 65.0) {
    lv_img_set_src(_battery_icon, LV_SYMBOL_BATTERY_3);
  } else if (sensor_readings.battery_percentage > 35.0) {
    lv_img_set_src(_battery_icon, LV_SYMBOL_BATTERY_2);
  } else if (sensor_readings.battery_percentage > 5.0) {
    lv_img_set_src(_battery_icon, LV_SYMBOL_BATTERY_1);
  } else {
    lv_img_set_src(_battery_icon, LV_SYMBOL_BATTERY_EMPTY);
  }

  lv_color_t color;
  if (sensor_readings.battery_percentage > 60.0) {
    color = LV_COLOR_GREEN;
  } else if (sensor_readings.battery_percentage > 20.0) {
    color = LV_COLOR_YELLOW;
  } else {
    color = LV_COLOR_RED;
  }
  lv_obj_set_style_local_image_recolor(_battery_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, color);

  if (digitalRead(SD_DETECT)) {
    lv_obj_set_style_local_image_recolor(_sd_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
  } else {
    lv_obj_set_style_local_image_recolor(_sd_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  }

  if (WiFi.status() == WL_CONNECTED) {
    lv_obj_set_style_local_image_recolor(_wifi_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
  } else {
    lv_obj_set_style_local_image_recolor(_wifi_icon, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  }
}


void App::close_event_handler(lv_obj_t * obj, lv_event_t event)
{
  current_app->deactivate();
  current_app = idle;
  idle->activate();
}


void App::hide()
{
  lv_obj_set_hidden(_window, true);
}


void App::show()
{
  lv_obj_set_hidden(_window, false);
}
