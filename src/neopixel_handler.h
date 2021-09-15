// Neopixel logging handler (mainly for release mode)
//
// Copyright (c) 2020 Dave Astels

#ifndef __NEOPIXEL_HANDLER_H__
#define __NEOPIXEL_HANDLER_H__

#include "logging_handler.h"
#include "indicator.h"

class NeopixelHandler: public LoggingHandler
{
public:
  NeopixelHandler(Indicator *pixel);
  void emit(const char *level_name, const char *msg);
  const char* name(void) { return "neopixel"; }
  void clear(void);
private:
  Indicator *indicator;
};


#endif
