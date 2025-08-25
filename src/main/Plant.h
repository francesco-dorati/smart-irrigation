#ifndef PLANT_H
#define PLANT_H

#include <Arduino.h>

#include "RS485Server.h"

/*
(target humidity, water humidity, water needs)
Water Requirements (ml per week baseline)
    Small plants (4-inch pot): 50-150ml depending on type
    Medium plants (6-8 inch pot): 200-500ml
    Large plants (10+ inch pot): 500-1200ml

Soil Moisture Levels
    Succulents/Cacti: Water when soil drops to 10-15%, target 70-80% after
watering Tropical Plants: Maintain 40-60%, water when drops to 35% Mediterranean
Plants: Water when drops to 20-25%, allow moderate drying Ferns/Moisture-loving:
Maintain 50-70%, never below 40%

Advanced Adjustments
    Seasonal Factor: Reduce watering by 30-50% during dormant months
    Environmental Factor: Increase by 20-40% during hot, dry periods
    Growth Stage Factor: Increase by 25% during active growing season
Environmental Thresholds
    Temperature: Monitor 15-30°C range, increase watering frequency above 25°C
    Humidity: Below 40% increases water demand significantly
    Light Intensity: Bright light (>2000 lux) increases transpiration rates

*/

enum PlantState { ABSORBING, WATERING };
class WaterPreference {
 public:
  float minHumidity;
  float optimalHumidity;
  String name;

  WaterPreference(float minHumidity, float optimalHumidity, String name = "CUSTOM");

  String toString() const;
  static WaterPreference fromValues(String name, float minH, float optH);

  static const WaterPreference SUCCULENT;
  static const WaterPreference FERN;
  static const WaterPreference HOUSEPLANT;
  static const WaterPreference MEDITERRANEAN;
  static const WaterPreference VEGETABLE;

  static const WaterPreference* allTypes[];

};

// enum SunlightExposure { INDOOR, NO_SUN, HALF_SUN, FULL_SUN };

class Plant {
 public:
  Plant(RS485Server& server, int id, String name, PlantState state,
        WaterPreference waterPreference, int saucerCapacity);

  int getId() const;
  String getName() const;
  bool ping();
  bool loadStatus();

  float getHumidity();
  bool isSaucerFull();
  bool water(int seconds);
  int checkWaterNeeds(float humidity);

 private:
  RS485Server& server;
  int id;       // Unique identifier for the plant
  String name;  // Name of the plant
  int saucerCapacity;

  PlantState state;
  float humidity;
  bool saucerFull;

  WaterPreference waterPreference;
};

#endif  // PLANT_H