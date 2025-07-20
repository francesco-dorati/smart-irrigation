#include "LED.h"

LED::LED(int p) : pin(p) {}

void LED::begin() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void LED::setOn() { digitalWrite(pin, HIGH); }

void LED::setOff() { digitalWrite(pin, LOW); }

void LED::blink() {
  setOn();
  delay(500);
  setOff();
}
