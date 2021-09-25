// -*- mode: c++ -*-

// global definitions
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

#ifndef __DEFINES_H__
#define __DEFINES_H__

#define CENTER_BUTTON_PIN (24)
#define UP_BUTTON_PIN      (3)
#define DOWN_BUTTON_PIN    (2)
#define LEFT_BUTTON_PIN   (11)
#define RIGHT_BUTTON_PIN  (25)

#define CENTER_BUTTON_MASK (1 << CENTER_BUTTON_PIN)
#define UP_BUTTON_MASK     (1 << UP_BUTTON_PIN)
#define DOWN_BUTTON_MASK   (1 << DOWN_BUTTON_PIN)
#define LEFT_BUTTON_MASK   (1 << LEFT_BUTTON_PIN)
#define RIGHT_BUTTON_MASK  (1 << RIGHT_BUTTON_PIN)

#define NEOPIXEL_PIN      (15)

#define RED_LED       (13)
#define TFT_RESET     (24)
#define TFT_BACKLIGHT (25)
#define LIGHT_SENSOR  (A2)
#define SD_CS         (32)
#define SPKR_SHUTDOWN (50)

#define TFT_ROTATION   (1) // Landscape orientation on PyPortal
#define TFT_D0        (34) // Data bit 0 pin (MUST be on PORT byte boundary)
#define TFT_WR        (26) // Write-strobe pin (CCL-inverted timer output)
#define TFT_DC        (10) // Data/command pin
#define TFT_CS        (11) // Chip-select pin
#define TFT_RST       (24) // Reset pin
#define TFT_RD         (9) // Read-strobe pin
#define TFT_BACKLIGHT (25)

#define YP (A4)  // must be an analog pin, use "An" notation!
#define XM (A7)  // must be an analog pin, use "An" notation!
#define YM (A6)   // can be a digital pin
#define XP (A5)   // can be a digital pin

#define X_MIN  (750)
#define X_MAX  (325)
#define Y_MIN  (840)
#define Y_MAX  (240)

#endif
