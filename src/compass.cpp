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
#include "compass.h"

extern lv_font_t dseg7_32;

Compass *compass_instance;
long int display_update_time = 0;

LV_IMG_DECLARE(compass_image);

Compass::Compass()
  : App("Compass")
{
  compass_instance = this;
  _window = lv_win_create(lv_scr_act(), NULL);
  lv_obj_set_hidden(_window, true);
  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, LV_TXT_FLAG_CENTER);
  lv_win_set_title(_window, "");
  lv_win_set_scrollbar_mode(_window, LV_SCRLBAR_MODE_OFF);
  static lv_style_t window_style;
  lv_style_set_bg_color(&window_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_add_style(_window, LV_WIN_PART_BG, &window_style);

  _compass_image = lv_img_create(_window, NULL);
  lv_img_set_src(_compass_image, &compass_image);
  lv_obj_align(_compass_image, NULL, LV_ALIGN_CENTER, 0, 0);
}


Compass::~Compass()
{
}


void Compass::activate()
{
  App::activate();
  setMagUpdate(true);
}


void Compass::deactivate()
{
  setMagUpdate(false);
  App::deactivate();
}


float min_x, max_x, mid_x;
float min_y, max_y, mid_y;
float min_z, max_z, mid_z;

void calibrate()
{
  float x, y, z;

  x = sensor_readings.magnetic.x;
  y = sensor_readings.magnetic.y;
  z = sensor_readings.magnetic.z;


  Serial.print("Mag: (");
  Serial.print(x); Serial.print(", ");
  Serial.print(y); Serial.print(", ");
  Serial.print(z); Serial.print(")");

  min_x = min(min_x, x);
  min_y = min(min_y, y);
  min_z = min(min_z, z);

  max_x = max(max_x, x);
  max_y = max(max_y, y);
  max_z = max(max_z, z);

  mid_x = (max_x + min_x) / 2;
  mid_y = (max_y + min_y) / 2;
  mid_z = (max_z + min_z) / 2;
  Serial.print(" Hard offset: (");
  Serial.print(mid_x); Serial.print(", ");
  Serial.print(mid_y); Serial.print(", ");
  Serial.print(mid_z); Serial.print(")");

  Serial.print(" Field: (");
  Serial.print((max_x - min_x)/2); Serial.print(", ");
  Serial.print((max_y - min_y)/2); Serial.print(", ");
  Serial.print((max_z - min_z)/2); Serial.println(")");
  delay(10);
}


// Mag: (12.45, 19.35, -49.80) Hard offset: (6.22, 9.75, -24.90) Field: (6.22, 9.75, 24.90)

int16_t Compass::compute_heading()
{
  float x = sensor_readings.magnetic.x;
  float y = sensor_readings.magnetic.y;
  int16_t heading = (int16_t)((atan2(y, x) * 1800.0) / 3.14159); // This is the value minus the hard iron offset
  return heading + ((heading < 0) ? 3600 : 0);
}


void Compass::update_display()
{
  int16_t heading = compute_heading();
  logger->debug("Mag x: %6.2f y: %6.2f heading: %3d", sensor_readings.magnetic.x, sensor_readings.magnetic.y, heading);
  lv_img_set_angle(_compass_image, 3600 - heading);
}


void Compass::update(unsigned long now)
{
  // calibrate();
  if (now >= display_update_time) {
    display_update_time = now + 100; // update 10 times a second
    update_display();
  }
}



void Compass::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;

  logger->debug("Compass::event_handler");
  logger->debug("Event: %d", event);

  switch (event) {
  case LV_EVENT_CLICKED:
    break;
  case LV_EVENT_KEY:
    key_ptr = (uint32_t*)(lv_event_get_data());
    switch (*key_ptr) {
    case LV_KEY_ESC:
      App::close_event_handler(nullptr, 0);
      break;
    case LV_KEY_LEFT:
      break;
    case LV_KEY_RIGHT:
      break;
    case LV_KEY_PREV:
    case LV_KEY_UP:
      break;
    case LV_KEY_NEXT:
    case LV_KEY_DOWN:
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}
