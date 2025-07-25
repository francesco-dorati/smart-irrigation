#include "HumiditySensor.h"

HumiditySensor::HumiditySensor(int p) : pin(p) {}

void HumiditySensor::begin() { pinMode(pin, INPUT); }

float HumiditySensor::getHumidity() {
  return (getVoltage() - DRY_VOLTAGE) / (WET_VOLTAGE - DRY_VOLTAGE);
}

float HumiditySensor::getVoltage() {
  return (getValue() * MAX_VOLTAGE) / 1023.0;
}

int HumiditySensor::getValue() { return analogRead(pin); }
