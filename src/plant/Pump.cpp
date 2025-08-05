#include "Pump.h"

Pump::Pump(int p) : pin(p), on(false) {}

void Pump::begin() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);  // Start with pump OFF
}

void Pump::setOn() {
  digitalWrite(pin, HIGH);  // Turn pump ON
  on = true;
}

void Pump::setOff() {
  digitalWrite(pin, LOW);  // Turn pump OFF
  on = false;
}

bool Pump::isOn() { return on; }

void Pump::activate(int seconds) {
  setOn();
  delay(seconds * 1000);
  setOff();
}
