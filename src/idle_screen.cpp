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

#include "defines.h"
#include "idle_screen.h"
#include "globals.h"
#include "app_menu.h"

IdleScreen::IdleScreen()
  : App("Idle Screen")
{
  // build UI
  _window = lv_win_create(lv_scr_act(), NULL);
  lv_win_set_header_height(_window, 20);
  lv_win_title_set_alignment(_window, 1);
  lv_win_set_title(_window, "IDLE SCREEN");
  _app_menu = new AppMenu(_window);
}

bool IdleScreen::register_app(App *app)
{
  return _app_menu->register_app(app);
}


void IdleScreen::update()
{
}


void IdleScreen::nav_button_pressed(uint8_t button)
{
}


void IdleScreen::nav_button_released(uint8_t button)
{
  switch (button) {
  case CENTER_BUTTON_PIN:
    logger->debug("Center button released in IdleScreen");
    _app_menu->choose_app();
    break;
  default:
    break;
  }
}
