// -*- mode: c++ -*-

// Nav pad support: encoder + U/D/L/R keypad
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

#include <lvgl.h>

#include "defines.h"
#include "Adafruit_seesaw.h"
#include "globals.h"


extern Adafruit_seesaw ss;
extern uint32_t button_pin_mask;

lv_indev_t *encoder_dev;
lv_indev_t *navpad_dev;

//==============================================================================
// Encoder support: dial and center button

bool encoder_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
  //logger->debug("encoder_read");
  data->enc_diff = (int16_t)ss.getEncoderDelta();
  // logger->debug("encoder read, diff: %d", data->enc_diff);
  data->state = ss.digitalRead(CENTER_BUTTON_PIN) ? LV_INDEV_STATE_REL : LV_INDEV_STATE_PR;
  return false;
}


//==============================================================================
// Direction pad support

typedef struct key_change {
  uint8_t key;
  bool pressed;
  struct key_change *next;
} key_change_t;

key_change_t *keys = nullptr;

void insert_key(key_change_t *k)
{
  key_change_t *p = keys;
  while (p && p->next) {
    p = p->next;
  }
  if (p) {
    p->next = k;
  } else {
    keys = k;
  }
}


uint32_t old_buttons = 0xFFFFFFFF;

void read_keys()
{
  uint32_t buttons = ss.digitalReadBulk(button_pin_mask);
  uint32_t mask = 0x00000001;
  for (uint8_t pin = 0; pin < 32; pin++) {
    if (pin  != CENTER_BUTTON_PIN) { // handler by encoder
      if (!(buttons & mask) && (old_buttons & mask)) {
        key_change_t *k = new key_change_t;
        k->key = pin;
        k->pressed = true;
        k->next = nullptr;
        insert_key(k);
      } else if ((buttons & mask) && !(old_buttons & mask)) {
        key_change_t *k = new key_change_t;
        k->key = pin;
        k->pressed = false;
        k->next = nullptr;
        insert_key(k);
      }
    }
    mask <<= 1;
  }
  old_buttons = buttons;
}


bool navpad_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
  //  logger->debug("navpad_read");
  if (!keys) {
    read_keys();
  }
  if (keys) {
    key_change_t *key_change = keys;
    keys = keys->next;
    switch (key_change->key) {
    case UP_BUTTON_PIN:
      data->key = LV_KEY_UP;
      break;
    case DOWN_BUTTON_PIN:
      data->key = LV_KEY_DOWN;
      break;
    case LEFT_BUTTON_PIN:
      data->key = LV_KEY_LEFT;
      break;
    case RIGHT_BUTTON_PIN:
      data->key = LV_KEY_RIGHT;
      break;
    }
    data->state = key_change->pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    delete key_change;
  } else {
    data->key = 0;
  }
  return false; /*No buffering now so no more data read*/
}


//==============================================================================
// Initialize the nav pad support

void initialize_nav_pad()
{
  lv_indev_drv_t encoder_drv;
  lv_indev_drv_init(&encoder_drv);
  encoder_drv.type = LV_INDEV_TYPE_ENCODER;
  encoder_drv.read_cb = encoder_read;
  encoder_dev = lv_indev_drv_register(&encoder_drv);

  lv_indev_drv_t navpad_drv;
  lv_indev_drv_init(&navpad_drv);
  navpad_drv.type = LV_INDEV_TYPE_KEYPAD;
  navpad_drv.read_cb = navpad_read;
  navpad_dev = lv_indev_drv_register(&navpad_drv);
}
