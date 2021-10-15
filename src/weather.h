// -*- mode: c++ -*-

// Weather App
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

#ifndef __WEATHER_H__
#define __WEATHER_H__

#include <WiFiNINA.h>

#include "app.h"

class Weather: public App
{
public:
  Weather();
  ~Weather();
  void update(unsigned long now);
  void activate();
  void deactivate();
  void handle(lv_obj_t * obj, lv_event_t event) { Weather::event_handler(obj, event); }

private:
  void update_display();
  bool fetch_weather();
  bool extract_data();
  WiFiClient *_client;
  static void event_handler(lv_obj_t * obj, lv_event_t event);

  char *_description;
  float _temperature;
  float _feels_like;
  float _pressure;
  float _humidity;
  float _wind_speed;
  float _wind_direction;
  float _wind_gust;

  lv_obj_t *_description_display;
  lv_obj_t *_temperature_display;
  lv_obj_t *_temperature_feels_like_display;
  lv_obj_t *_pressure_display;
  lv_obj_t *_humidity_display;
  lv_obj_t *_wind_speed_display;
  lv_obj_t *_wind_direction_display;
  lv_obj_t *_wind_gust_display;
  unsigned long _screen_update_time;

};

#endif
