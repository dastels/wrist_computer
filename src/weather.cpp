// -*- mode: c++ -*-

// Weather fetch/process
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
#include <ArduinoJson.h>

#include "defines.h"
#include "secrets.h"
#include "globals.h"
#include "weather.h"

extern lv_font_t dseg7_16;

int status = WL_IDLE_STATUS;

Weather *weather_instance;


Weather::Weather()
  : App("Weather")
  , _client(new WiFiClient())
  , _screen_update_time(0)
{
  weather_instance = this;

  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);
  lv_group_t * _group = lv_group_create();
  lv_group_add_obj(_group, _window);

  lv_obj_set_hidden(_window, true);

  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, LV_TXT_FLAG_CENTER);
  lv_win_set_title(_window, "");
  lv_win_set_scrollbar_mode(_window, LV_SCRLBAR_MODE_OFF);

  static lv_style_t window_style;
  lv_style_set_bg_color(&window_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_add_style(_window, LV_WIN_PART_BG, &window_style);

  //==============================================================================
  // Description display
  //==============================================================================

  lv_obj_t *_condition_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_condition_label, true);
  lv_obj_align(_condition_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 25);
  lv_label_set_text(_condition_label, "#AAAAAA Condition#");

  _description_display = lv_label_create(_window, NULL);
  lv_label_set_recolor(_description_display, true);
  lv_obj_align(_description_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 25);
  lv_label_set_text(_description_display, "");
  lv_label_set_recolor(_description_display, true);

  //==============================================================================
  // Temperature display
  //==============================================================================

  lv_obj_t *_temperature_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_temperature_label, true);
  lv_obj_align(_temperature_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 50);
  lv_label_set_text(_temperature_label, "#AAAAAA Temperature#");

  _temperature_display = lv_label_create(_window, NULL);
  lv_obj_align(_temperature_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 50);
  lv_label_set_text(_temperature_display, "");
  lv_label_set_recolor(_temperature_display, true);
  static lv_style_t temperature_style;
  lv_style_set_text_font(&temperature_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_temperature_display, LV_LABEL_PART_MAIN, &temperature_style);

  //==============================================================================
  // Temperature Feels Like display
  //==============================================================================

  lv_obj_t *_temperature_feels_like_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_temperature_feels_like_label, true);
  lv_obj_align(_temperature_feels_like_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 75);
  lv_label_set_text(_temperature_feels_like_label, "#AAAAAA Feels Like#");

  _temperature_feels_like_display = lv_label_create(_window, NULL);
  lv_obj_align(_temperature_feels_like_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 75);
  lv_label_set_text(_temperature_feels_like_display, "");
  lv_label_set_recolor(_temperature_feels_like_display, true);
  static lv_style_t temperature_feels_like_style;
  lv_style_set_text_font(&temperature_feels_like_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_temperature_feels_like_display, LV_LABEL_PART_MAIN, &temperature_feels_like_style);

  //==============================================================================
  // Humidity display
  //==============================================================================

  lv_obj_t *_humidity_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_humidity_label, true);
  lv_obj_align(_humidity_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 100);
  lv_label_set_text(_humidity_label, "#AAAAAA Humidity#");

  _humidity_display = lv_label_create(_window, NULL);
  lv_obj_align(_humidity_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 100);
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
  lv_obj_align(_pressure_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 125);
  lv_label_set_text(_pressure_label, "#AAAAAA Pressure#");

  _pressure_display = lv_label_create(_window, NULL);
  lv_obj_align(_pressure_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 125);
  lv_label_set_text(_pressure_display, "");
  lv_label_set_recolor(_pressure_display, true);
  static lv_style_t pressure_style;
  lv_style_set_text_font(&pressure_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_pressure_display, LV_LABEL_PART_MAIN, &pressure_style);

  //==============================================================================
  // Wind Speed display
  //==============================================================================

  lv_obj_t *_wind_speed_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_wind_speed_label, true);
  lv_obj_align(_wind_speed_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 150);
  lv_label_set_text(_wind_speed_label, "#AAAAAA Wind Speed#");

  _wind_speed_display = lv_label_create(_window, NULL);
  lv_obj_align(_wind_speed_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 150);
  lv_label_set_text(_wind_speed_display, "");
  lv_label_set_recolor(_wind_speed_display, true);
  static lv_style_t wind_speed_style;
  lv_style_set_text_font(&wind_speed_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_wind_speed_display, LV_LABEL_PART_MAIN, &wind_speed_style);

  //==============================================================================
  // Wind Direction display
  //==============================================================================

  lv_obj_t *_wind_direction_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_wind_direction_label, true);
  lv_obj_align(_wind_direction_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 175);
  lv_label_set_text(_wind_direction_label, "#AAAAAA     Direction#");

  _wind_direction_display = lv_label_create(_window, NULL);
  lv_obj_align(_wind_direction_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 175);
  lv_label_set_text(_wind_direction_display, "");
  lv_label_set_recolor(_wind_direction_display, true);

  //==============================================================================
  // Wind Gust display
  //==============================================================================

  lv_obj_t *_wind_gust_label = lv_label_create(_window, NULL);
  lv_label_set_recolor(_wind_gust_label, true);
  lv_obj_align(_wind_gust_label, _window, LV_ALIGN_IN_TOP_LEFT, 5, 200);
  lv_label_set_text(_wind_gust_label, "#AAAAAA     Gust#");

  _wind_gust_display = lv_label_create(_window, NULL);
  lv_obj_align(_wind_gust_display, _window, LV_ALIGN_IN_TOP_LEFT, 105, 200);
  lv_label_set_text(_wind_gust_display, "");
  lv_label_set_recolor(_wind_gust_display, true);
  static lv_style_t wind_gust_style;
  lv_style_set_text_font(&wind_gust_style, LV_STATE_DEFAULT, &dseg7_16);
  lv_obj_add_style(_wind_gust_display, LV_LABEL_PART_MAIN, &wind_gust_style);

}


Weather::~Weather()
{
}


void Weather::activate()
{
  App::activate();
  if (fetch_weather()) {
    logger->debug("Got weather data");
    if (extract_data()) {
      logger->debug("Refreshed weather data");
    } else {
      logger->debug("Error extracting weather data");
    }
  } else {
    logger->debug("Error fetching weather data");
  }
}


void Weather::deactivate()
{
  App::deactivate();
}


void Weather::update_display()
{
}


char *wind_directions[] = {"North", "North East", "East", "South East", "South", "South West", "West", "North West", "Unknown"};

char *Weather::wind_degrees_to_direction(float direction)
{
  logger->debug("Wind degrees: %f", direction);
  if (direction >= 337.5 || direction < 22.5) {
    return wind_directions[0];
  } else if (direction >= 22.5 && direction < 67.5) {
    return wind_directions[1];
  } else if (direction >= 67.5 && direction < 112.5) {
    return wind_directions[2];
  } else if (direction >= 112.5 && direction < 157.5) {
    return wind_directions[3];
  } else if (direction >= 157.5 && direction < 202.5) {
    return wind_directions[4];
  } else if (direction >= 202.5 && direction < 247.5) {
    return wind_directions[5];
  } else if (direction >= 247.5 && direction < 292.5) {
    return wind_directions[6];
  } else if (direction >= 292.5 && direction < 337.5) {
    return wind_directions[7];
  } else {
    return wind_directions[8];
  }
}


void Weather::update(unsigned long now)
{
    if (now >= _screen_update_time) {
    _screen_update_time = now + 60000;

    sprintf(strbuf, "#FFFF00 %s#", _description);
    lv_label_set_text(_description_display, strbuf);

    sprintf(strbuf, "#%s %07.2f# #888888#", (_temperature > 30.0 ? "FF0000" : "00FF00"), _temperature);
    lv_label_set_text(_temperature_display, strbuf);

    sprintf(strbuf, "#%s %07.2f# #888888#", (_feels_like > 30.0 ? "FF0000" : "00FF00"), _feels_like);
    lv_label_set_text(_temperature_feels_like_display, strbuf);

    sprintf(strbuf, "#00FF00 %07.2f#", _humidity);
    lv_label_set_text(_humidity_display, strbuf);

    sprintf(strbuf, "#00FF00 %07.2f#", _pressure);
    lv_label_set_text(_pressure_display, strbuf);

    sprintf(strbuf, "#00FF00 %07.2f#", _wind_speed);
    lv_label_set_text(_wind_speed_display, strbuf);

    sprintf(strbuf, "#00FF00 From the %s#", wind_degrees_to_direction(_wind_direction));
    lv_label_set_text(_wind_direction_display, strbuf);

    sprintf(strbuf, "#00FF00 %07.2f#", _wind_gust);
    lv_label_set_text(_wind_gust_display, strbuf);
  }

}

// Example response data:
// {
//   "coord": {
//     "lon": -81.2497,
//     "lat": 42.9837
//   },
//   "weather": [
//     {
//       "id": 701,
//       "main": "Mist",
//       "description": "mist",
//       "icon": "50d"
//     }
//   ],
//   "base": "stations",
//   "main": {
//     "temp": 15.25,
//     "feels_like": 15.08,
//     "temp_min": 12.87,
//     "temp_max": 16.26,
//     "pressure": 1018,
//     "humidity": 86
//   },
//   "visibility": 9656,
//   "wind": {
//     "speed": 2.68,
//     "deg": 103,
//     "gust": 3.58
//   },
//   "clouds": {
//     "all": 75
//   },
//   "dt": 1633526273,
//   "sys": {
//     "type": 2,
//     "id": 20399,
//     "country": "CA",
//     "sunrise": 1633519685,
//     "sunset": 1633561062
//   },
//   "timezone": -14400,
//   "id": 6058560,
//   "name": "London",
//   "cod": 200
// }

bool Weather::fetch_weather()
{
  const char *server = "api.openweathermap.org";
  sprintf(strbuf, "GET /data/2.5/weather?lat=%s&lon=%s&units=metric&appid=%s", latitude, longitude, openweather_key);

  logger->debug("connecting to server: %s", server);
  if (_client->connect(server, 80)) {
    logger->debug("Connected to server");
    logger->debug("Getting %s", strbuf);
    _client->println(strbuf);
    _client->println();
    delay(250);

    while (!_client->available()) {
    }
    int chars_read = _client->read((uint8_t*)strbuf, 4096);
    if (chars_read > 0) {
      strbuf[chars_read] = 0;
      logger->debug("Read %d characters", chars_read);
      logger->debug("data length: %d", strlen(strbuf));
      return true;
    } else {
      logger->debug("Got no data");
      return false;
    }
  } else {
    logger->debug("Could not connect");
    return false;
  }
}



bool Weather::extract_data()
{
  logger->debug("Deserializing JSON");
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, strbuf);
  if (error) {
    logger->error("weather data deserialization failed: ");
    logger->error(error.c_str());
    return false;
  }
  logger->debug("Extracting data");
  const char *desc = doc["weather"][0]["description"];
  _description = (char *)desc;
  _temperature = doc["main"]["temp"];
  _feels_like = doc["main"]["feels_like"];
  _pressure = doc["main"]["pressure"];
  _humidity = doc["main"]["humidity"];
  _wind_speed = doc["wind"]["speed"];
  _wind_direction = doc["wind"]["deg"];
  _wind_gust = doc["wind"]["gust"];
  return true;
}


void Weather::event_handler(lv_obj_t * obj, lv_event_t event)
{
  uint32_t *key_ptr;

  logger->debug("Weather::event_handler");
  logger->debug("Event: %d", event);

  switch (event) {
  case LV_EVENT_KEY:
    key_ptr = (uint32_t*)(lv_event_get_data());
    logger->debug("Key event: %d", *key_ptr);
    switch (*key_ptr) {
    case LV_KEY_ESC:
      App::close_event_handler(nullptr, 0);
      break;
    default:
      break;
    }
  default:
    break;
  }

}
