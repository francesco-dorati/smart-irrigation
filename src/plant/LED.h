#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED {
 public:
  LED(int p);
  void begin();
  void setOn();
  void setOff();
  void blink();

 private:
  int pin;
};

#endif  // LED_H