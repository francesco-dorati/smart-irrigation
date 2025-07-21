#include "SaucerSensor.h"

SaucerSensor::SaucerSensor(int powerPin, int readingPin)
    : powerPin(powerPin), readingPin(readingPin) {}

void SaucerSensor::begin() {
  pinMode(powerPin, OUTPUT);
  pinMode(readingPin, INPUT);
  digitalWrite(powerPin, LOW);
}

bool SaucerSensor::isFull() {
  digitalWrite(powerPin, HIGH);
  delay(100);
  bool full = digitalRead(readingPin) == HIGH;
  digitalWrite(powerPin, LOW);
  return full;
}
