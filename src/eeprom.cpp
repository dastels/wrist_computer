// -*- mode: c++ -*-

// Seesaw EEPROM encapsulation
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

#include "eeprom.h"

// Data addresses
#define FRAMES_PER_SECOND (0)
#define STOPWATCH_MODE    (1)
#define SILENT_SETTING    (2)

Eeprom::Eeprom(Adafruit_seesaw *ss)
  : _ss(ss)
{
}


uint8_t Eeprom::get_stopwatch_mode()
{
  return _ss->EEPROMRead8(STOPWATCH_MODE);
}


void Eeprom::set_stopwatch_mode(uint8_t mode)
{
  _ss->EEPROMWrite8(STOPWATCH_MODE, mode);
}


uint8_t Eeprom::get_frames_per_second()
{
  return _ss->EEPROMRead8(FRAMES_PER_SECOND);
}


void Eeprom::set_frames_per_second(uint8_t fps)
{
  _ss->EEPROMWrite8(FRAMES_PER_SECOND, fps);
}


bool Eeprom::get_silent()
{
  return _ss->EEPROMRead8(SILENT_SETTING) != 0;
}


void Eeprom::set_silent(bool value)
{
  _ss->EEPROMWrite8(SILENT_SETTING, value ? 1 : 0);
}
