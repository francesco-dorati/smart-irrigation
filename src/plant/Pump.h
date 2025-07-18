#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>

class Pump {
 public:
  Pump(int p);
  void begin();
  void setOn();
  void setOff();
  bool isOn();
  void activate(int seconds);

 private:
  int pin;
  bool on;
};

#endif  // PUMP_H