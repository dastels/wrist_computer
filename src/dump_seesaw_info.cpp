// -*- mode: c++ -*-

// Dump SeeSaw info/config
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

#include <Adafruit_seesaw.h>
#include <LibPrintf.h>

void dump_seesaw_info(Adafruit_seesaw *ss)
{
  printf("Version: %d\n", ss->getVersion());
  uint32_t options = ss->getOptions();
  printf("Options:\n");
  if ((options & (0x01 << SEESAW_STATUS_BASE)) > 0) {
    printf("  STATUS\n");
  }
  if ((options & (0x01 << SEESAW_GPIO_BASE)) > 0) {
    printf("  GPIO\n");
  }
  if ((options & (0x01 << SEESAW_SERCOM0_BASE)) > 0) {
    printf("  SERCOM\n");
  }
  if ((options & (0x01 << SEESAW_TIMER_BASE)) > 0) {
    printf("  TIMER\n");
  }
  if ((options & (0x01 << SEESAW_ADC_BASE)) > 0) {
    printf("  ADC\n");
  }
  if ((options & (0x01 << SEESAW_DAC_BASE)) > 0) {
    printf("  DAC\n");
  }
  if ((options & (0x01 << SEESAW_INTERRUPT_BASE)) > 0) {
    printf("  INTERRUPT\n");
  }
  if ((options & (0x01 << SEESAW_DAP_BASE)) > 0) {
    printf("  DAP\n");
  }
  if ((options & (0x01 << SEESAW_EEPROM_BASE)) > 0) {
    printf("  EEPROM\n");
  }
  if ((options & (0x01 << SEESAW_NEOPIXEL_BASE)) > 0) {
    printf("  NEOPIXEL\n");
  }
  if ((options & (0x01 << SEESAW_TOUCH_BASE)) > 0) {
    printf("  TOUCH\n");
  }
  if ((options & (0x01 << SEESAW_KEYPAD_BASE)) > 0) {
    printf("  KEYPAD\n");
  }
  if ((options & (0x01 << SEESAW_ENCODER_BASE)) > 0) {
    printf("  ENCODER\n");
  }
}
