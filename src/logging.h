// -*- mode: c++ -*-
// Logger
//
// Copyright (c) 2020 Dave Astels

#ifndef __LOGGING_H__
#define __LOGGING_H__

enum class LogLevel : int { NOTSET, DEBUG_DEEP, DEBUG_MID, DEBUG, INFO, WARNING, ERROR, CRITICAL };

#include <stdarg.h>
#include "logging_handler.h"

#ifndef MAX_NUMBER_OF_HANDLERS
#define MAX_NUMBER_OF_HANDLERS (4)
#endif

class Logger
{
  static Logger *_instance;
  LoggingHandler *_handlers[MAX_NUMBER_OF_HANDLERS];
  LogLevel _level;
  char _buffer[256];
  void _internal_log(LogLevel level, const char *format, va_list args);

 public:
  Logger();
  Logger(LoggingHandler *handler);
  static Logger *get_logger(LoggingHandler *handler);
  static Logger *get_logger(void);
  LogLevel get_level() { return _level; }
  void set_level(LogLevel new_level);
  void set_handler(LoggingHandler *_handler);
  bool add_handler(LoggingHandler *_handler);
  bool initialized() { return _handlers[0]->initialized(); }
  void report(void);
  void log(LogLevel level, const char *format, ...);
  void debug_deep(const char *format, ...);
  void debug_mid(const char *format, ...);
  void debug(const char *format, ...);
  void info(const char *format, ...);
  void warning(const char *format, ...);
  void error(const char *format, ...);
  void critical(const char *format, ...);
  void clear(void);
};

const char *level_name_for(LogLevel level);
LogLevel level_for(const char *level_name);


#endif
