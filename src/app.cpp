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

#include "globals.h"
#include "app.h"


App::App(const char *name, bool make_persistant)
  :_name(name)
  , _persistant(make_persistant)
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
  sprintf(strbuf, "%4d/%02d/%02d %02d:%02d:%02d", now->year(), now->month(), now->day(), now->hour(), now->minute(), now->second());
  lv_win_set_title(_window, strbuf);
}


void App::close_event_handler(lv_obj_t * obj, lv_event_t event)
{
  current_app->deactivate();
  current_app = idle;
  idle->activate();
}
