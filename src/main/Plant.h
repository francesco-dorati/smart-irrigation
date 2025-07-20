#ifndef PLANT_H
#define PLANT_H

#include <Arduino.h>

#include "RS485Server.h"

enum WaterPreference {
  DRY = 0,  // Dry soil
  LOW_NEED = 15,
  MEDIUM_NEED = 30,
  HIGH_NEED = 50
};

enum SunlightExposure { INDOOR, NO_SUN, HALF_SUN, FULL_SUN };

class Plant {
 public:
  Plant(RS485Server& server, String id, String name, String lastWatered,
        WaterPreference waterPreference, SunlightExposure exposure);

  String getId() const;
  String getName() const;
  bool ping();
  int getHumidity();
  bool water(int seconds);
  String getLastWatered();
  int getWaterNeeds(int humidity);

 private:
  RS485Server& server;
  String id;    // Unique identifier for the plant
  String name;  // Name of the plant
  String lastWatered;
  WaterPreference waterPreference;
  SunlightExposure exposure;
  int potLiters;
  // also temperature (get from outside!!!!!!!)
  // max water per watering
  // Water Need (ml) = Base Water × Sunlight Factor × (Target Humidity - Current
  // Humidity) × Pot Size Factor
};

#endif  // PLANT_H