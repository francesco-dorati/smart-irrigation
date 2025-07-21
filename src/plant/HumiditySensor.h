#ifndef HUMIDITYSENSOR_H
#define HUMIDITYSENSOR_H

#include <Arduino.h>

class HumiditySensor {
 public:
  HumiditySensor(int p);
  void begin();
  float getHumidity();
  float getVoltage();
  int getValue();

 private:
  int pin;
  const float MAX_VOLTAGE = 5.0;
  const float DRY_VOLTAGE = 3.27;  // 3.04 on local
  const float WET_VOLTAGE = 1.18;  // 1.11 on local
};

#endif  // HUMIDITYSENSOR_H