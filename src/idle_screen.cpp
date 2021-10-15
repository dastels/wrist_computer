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

#include <lvgl.h>

#include "defines.h"
#include "idle_screen.h"
#include "globals.h"
#include "app_menu.h"

extern lv_font_t dseg7_16;

IdleScreen::IdleScreen()
  : App("Idle Screen")
  , _choosing_app(false)
  , _screen_update_time(0)
{
  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(_window, IdleScreen::event_handler);
  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, LV_TXT_FLAG_CENTER);
  //  lv_win_set_title(_window, "IDLE SCREEN");
  lv_win_set_scrollbar_mode(_window, LV_SCRLBAR_MODE_OFF);
  lv_obj_set_hidden(_window, false);

  static lv_style_t window_style;
  lv_style_set_bg_color(&window_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_add_style(_window, LV_WIN_PART_BG, &window_style);


  //==============================================================================
  // Temperature display
  //==============================================================================

  lv_obj_t *_temperature_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_temperature_label, true);
  lv_obj_align(_temperature_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 25);
  lv_label_set_text(_temperature_label, "#AAAAAA Temperature#");

  _temperature_display = lv_label_create(_window, NULL);
  lv_obj_align(_temperature_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 25);
  lv_label_set_text(_temperature_display, "");
  lv_label_set_recolor(_temperature_display, true);
  static lv_style_t temperature_style;
  lv_style_set_text_font(&temperature_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_temperature_display, LV_LABEL_PART_MAIN, &temperature_style);


  //==============================================================================
  // Humidity display
  //==============================================================================

  lv_obj_t *_humidity_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_humidity_label, true);
  lv_obj_align(_humidity_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 50);
  lv_label_set_text(_humidity_label, "#AAAAAA Humidity#");

  _humidity_display = lv_label_create(_window, NULL);
  lv_obj_align(_humidity_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 50);
  lv_label_set_text(_humidity_display, "");
  lv_label_set_recolor(_humidity_display, true);
  static lv_style_t humidity_style;
  lv_style_set_text_font(&humidity_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_humidity_display, LV_LABEL_PART_MAIN, &humidity_style);

  //==============================================================================
  // Pressure display
  //==============================================================================

  lv_obj_t *_pressure_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_pressure_label, true);
  lv_obj_align(_pressure_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 75);
  lv_label_set_text(_pressure_label, "#AAAAAA Pressure#");

  _pressure_display = lv_label_create(_window, NULL);
  lv_obj_align(_pressure_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 75);
  lv_label_set_text(_pressure_display, "");
  lv_label_set_recolor(_pressure_display, true);
  static lv_style_t pressure_style;
  lv_style_set_text_font(&pressure_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_pressure_display, LV_LABEL_PART_MAIN, &pressure_style);

  //==============================================================================
  // Gas display
  //==============================================================================

  lv_obj_t *_gas_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_gas_label, true);
  lv_obj_align(_gas_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 100);
  lv_label_set_text(_gas_label, "#AAAAAA Gas#");

  _gas_display = lv_label_create(_window, NULL);
  lv_obj_align(_gas_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 100);
  lv_label_set_text(_gas_display, "");
  lv_label_set_recolor(_gas_display, true);
  static lv_style_t gas_style;
  lv_style_set_text_font(&gas_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_gas_display, LV_LABEL_PART_MAIN, &gas_style);

  _app_menu = new AppMenu(_window);

  // Connect encoder and nav-pad
  _group = lv_group_create();
  lv_group_add_obj(_group, _window);
  lv_indev_set_group(encoder_dev, _group);
  lv_indev_set_group(navpad_dev, _group);
  lv_indev_enable(encoder_dev, true);
  lv_indev_enable(navpad_dev, true);
  lv_obj_add_state(_window, LV_STATE_FOCUSED);
}


void IdleScreen::activate()
{
  App::activate();
  _choosing_app = false;
}


void IdleScreen::deactivate()
{
  logger->debug("Trying to deactivate idle screen... ignoring");
}


bool IdleScreen::register_app(App *app)
{
  logger->debug("Registering app: %s", app->name());
  return _app_menu->register_app(app);
}


void IdleScreen::update(unsigned long now)
{
  if (now >= _screen_update_time) {
    _screen_update_time = now + 60000;

    sprintf(strbuf, "#%s %06.2f#", (sensor_readings.temperature > 30.0 ? "FF0000" : "00FF00"), sensor_readings.temperature);
    lv_label_set_text(_temperature_display, strbuf);

    sprintf(strbuf, "#00FF00 %06.2f#", sensor_readings.humidity);
    lv_label_set_text(_humidity_display, strbuf);

    sprintf(strbuf, "#00FF00 %06.2f#", sensor_readings.pressure);
    lv_label_set_text(_pressure_display, strbuf);

    sprintf(strbuf, "#00FF00 %06.2f#", sensor_readings.gas);
    lv_label_set_text(_gas_display, strbuf);
  }
}


void IdleScreen::update_persistant_background_apps(unsigned long now)
{
  for (int app_index = 0; app_index < _app_menu->_number_of_apps; app_index++) {
    App *app = _app_menu->_apps[app_index];
    if (!is_current_app(app) && app->is_persistant()) {
      app->update(now);
    }
  }
}


void IdleScreen::update_time_display(DateTime *now)
{
  sprintf(strbuf, "%4d/%02d/%02d %02d:%02d:%02d", now->year(), now->month(), now->day(), now->hour(), now->minute(), now->second());
  lv_win_set_title(_window, strbuf);
}


void IdleScreen::event_handler(lv_obj_t * obj, lv_event_t event)
{
  logger->debug("IdleScreen::event_handler");
  logger->debug("Event: %d", event);
  if (idle->choosing_app()) {   // send to the app chooser?
    // logger->debug("Passing event to Appmenu");
    AppMenu::event_handler(obj, event);
  } else if (current_app != idle) { // send to the active app?
    // logger->debug("Passing event to %s", current_app->name());
    current_app->handle(obj, event);
  } else {                      // handle it here
    // lv_event_t code = lv_event_get_code(event);
    switch (event) {
    case LV_EVENT_CLICKED:
      idle->choosing_app(true);
      idle->_app_menu->choose_app();
      break;
    default:
      break;
    }
  }
}
