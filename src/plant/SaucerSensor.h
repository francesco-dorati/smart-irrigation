#ifndef SAUCERSENSOR_H
#define SAUCERSENSOR_H

#include <Arduino.h>

class SaucerSensor {
public:
  SaucerSensor(int powerPin, int readingPin);
  void begin();
  bool isFull();

private:
  int powerPin;
  int readingPin;
};

#endif
