// SD card logging handler
//
// Copyright (c) 2020 Dave Astels

#include <SPI.h>
#include <SdFat.h>
#include "sdcard_handler.h"

#define LOG_FNAME ("log.txt")

extern SdFat SD;

SDCardHandler::SDCardHandler(int chip_select)
  : LoggingHandler()
  , _chip_select(chip_select)
{
  pinMode(chip_select, OUTPUT);
  _success = SD.begin(_chip_select);
  if (_success && SD.exists(LOG_FNAME)) {
    SD.remove(LOG_FNAME);
  }
}


void SDCardHandler::emit(const char *level_name, const char *msg)
{
  if (_success) {
    File log_file = SD.open(LOG_FNAME, FILE_WRITE);
    log_file.println(format(level_name, msg));
    log_file.close();
  }
}
