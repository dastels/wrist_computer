// Serial logging handler (mainly for debug mode)
//
// Copyright (c) 2020 Dave Astels

#include <Arduino.h>
#include "logging.h"
#include "neopixel_handler.h"

NeopixelHandler::NeopixelHandler(Indicator *pixel)
  : LoggingHandler()
  , indicator(pixel)
{
}


void NeopixelHandler::emit(const char *level_name, const char *msg)
{
  switch  (log_level_for(level_name)) {
  case LogLevel::WARNING:
    indicator->show(0xFFFF00);
    break;
  case LogLevel::ERROR:
    indicator->show(0xFFA500);
    break;
  case LogLevel::CRITICAL:
    indicator->show(0xFF0000);
    break;
  default:
    break;
  }

}


void NeopixelHandler::clear(void)
{
  indicator->show(0x000000);
}
