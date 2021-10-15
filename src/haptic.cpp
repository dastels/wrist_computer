// -*- mode: c++ -*-

// Haptic encapsulation
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

#include "haptic.h"
#include "logging.h"

extern Logger *logger;

Haptic::Haptic()
  : _drv(new Adafruit_DRV2605())
  , _number_of_patterns(0)
  , _loaded(-1)
{
}


bool Haptic::begin()
{
  bool status = _drv->begin();
  if (status) {
    _drv->setMode(DRV2605_MODE_INTTRIG);
  }
  return status;
}

int8_t Haptic::add_effect(uint8_t pattern[])
{
  uint8_t length = 0;
  if (_number_of_patterns == 16) {
    return -1;
  }
  while (pattern[length++]);
  _patterns[_number_of_patterns] = (uint8_t*)malloc(length);
  memcpy(_patterns[_number_of_patterns], pattern, length);
  return _number_of_patterns++;
}


void Haptic::play(uint8_t id)
{
  if (id <_number_of_patterns) {
    if (id != _loaded) {
      _drv->selectLibrary(1);
      uint8_t i = 0;
      do {
        _drv->setWaveform(i, _patterns[id][i]);
      }
      while (_patterns[id][i++]);
      _loaded = id;
    }
    _drv->go();
  }
}
