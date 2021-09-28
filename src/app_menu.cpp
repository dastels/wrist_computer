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

#include <LibPrintf.h>
#include "globals.h"
#include "app_menu.h"

extern App *current_app;
AppMenu *app_menu_instance;

static uint32_t key_data;

AppMenu::AppMenu(lv_obj_t *window)
  : _number_of_apps(0)
  , _window(window)
{
  app_menu_instance = this;
  _app_roller = lv_roller_create(_window, NULL);
  lv_obj_set_hidden(_app_roller, true);
  lv_obj_align(_app_roller, _window, LV_ALIGN_CENTER, 0, 0);
  lv_roller_set_auto_fit(_app_roller, true);
  lv_obj_set_event_cb(_app_roller, AppMenu::event_handler);
  // lv_group_t * _group = lv_group_create();
  // lv_group_add_obj(_group, _app_roller);
  // lv_indev_set_group(encoder_dev, _group);
  // lv_indev_set_group(navpad_dev, _group);
  lv_indev_enable(encoder_dev, true);
  lv_indev_enable(navpad_dev, true);
}


void AppMenu::open()
{
  // logger->debug("Opening AppMenu");
  lv_obj_set_hidden(_app_roller, false);
  lv_event_send(_app_roller, LV_EVENT_FOCUSED, NULL);
  lv_task_handler();
}


void AppMenu::close()
{
  logger->debug("Closing AppMenu");
  lv_obj_set_hidden(_app_roller, true);
  lv_task_handler();
}


bool AppMenu::register_app(App *app)
{
  if (_number_of_apps < 16) {
    _apps[_number_of_apps++] = app;
    return true;
  }
  return false;
}


void AppMenu::choose_app()
{
  logger->debug("Entering AppMenu::choose_app()");
  char options[512];
  options[0] = 0;
  char option_buf[32];
  for (int i = 0; i < _number_of_apps; i++) {
    if (i == _number_of_apps - 1) {
      sprintf(option_buf, "%s", _apps[i]->name());
    } else {
      sprintf(option_buf, "%s\n", _apps[i]->name());
    }
    strcat(options, option_buf);
  }
  lv_roller_set_options(_app_roller, options, LV_ROLLER_MODE_NORMAL);
  lv_roller_set_visible_row_count(_app_roller, max(4, _number_of_apps));
  open();
}


void AppMenu::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;
  char buf[32];
  uint16_t selected;

  logger->debug("AppMenu::event_handler");
  logger->debug("Event: %d", event);

  switch (event) {
  case LV_EVENT_CLICKED:
    lv_roller_get_selected_str(app_menu_instance->_app_roller, buf, sizeof(buf));
    logger->debug("Selected app: %s", buf);
    for (int i = 0; i < app_menu_instance->_number_of_apps; i++) {
      if (strcmp(buf, app_menu_instance->_apps[i]->name()) == 0) {
        current_app = app_menu_instance->_apps[i];
        logger->debug("Chose `%s`", current_app->name());
        idle->choosing_app(false);
        app_menu_instance->close();
        current_app->activate();
        return;
      }
    }
    break;
  case LV_EVENT_KEY:
    key_ptr = (uint32_t*)(lv_event_get_data());
    switch (*key_ptr) {
    case LV_KEY_LEFT:           // cancel app selection and return to idle screen
      app_menu_instance->close();
      idle->activate();
      break;
    case LV_KEY_PREV:
    case LV_KEY_UP:
      selected = lv_roller_get_selected(app_menu_instance->_app_roller);
      if (selected > 0) {
        lv_roller_set_selected(app_menu_instance->_app_roller, selected - 1, LV_ANIM_ON);
      }
      break;
    case LV_KEY_NEXT:
    case LV_KEY_DOWN:
      selected = lv_roller_get_selected(app_menu_instance->_app_roller);
      if (selected < app_menu_instance->_number_of_apps) {
        lv_roller_set_selected(app_menu_instance->_app_roller, selected + 1, LV_ANIM_ON);
      }
      break;
    default:
      break;
    }
  default:
    break;
  }
}
