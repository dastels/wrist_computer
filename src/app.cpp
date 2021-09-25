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


App::App(const char *name)
  :_name(name)
{
}

App::~App()
{
}

void App::activate()
{
  logger->debug("Activating %s", _name);
   lv_obj_set_hidden(_window, false);
}


void App::deactivate()
{
  logger->debug("Deactivating %s", _name);
  lv_obj_set_hidden(_window, true);
}

void App::encoder_changed(int32_t pos)
{
}


void App::nav_button_pressed(uint8_t button)
{
}


void App::nav_button_released(uint8_t button)
{
}


void App::update() {
}


void App::close_event_handler(lv_obj_t * obj, lv_event_t event)
{
  current_app->deactivate();
  current_app = idle;
  idle->activate();
}
