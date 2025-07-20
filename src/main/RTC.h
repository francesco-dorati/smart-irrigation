#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
//#include <RTClib.h>


class RTC {
 public:
  RTC();
  bool begin();
  String getDateTime();
  void setDateTime(String dateTime);
 private:
  //RTC_DS3231 rtc;
};

#endif  // RTC_H