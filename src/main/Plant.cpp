#include "Plant.h"

WaterPreference::WaterPreference(float minHumidity, float optimalHumidity, String name = "CUSTOM")
    : minHumidity(minHumidity),
      optimalHumidity(optimalHumidity),
      name(name) {}

String WaterPreference::toString() const {
  return name + "," + String(minHumidity) + "," + String(optimalHumidity);
}

WaterPreference WaterPreference::fromValues(String name, float minH, float optH) {
  return WaterPreference(minH, optH, name);
}

const WaterPreference WaterPreference::SUCCULENT(0.15, 0.40, "SUCCULENT");
const WaterPreference WaterPreference::FERN(0.55, 0.85, "FERN");
const WaterPreference WaterPreference::HOUSEPLANT(0.35, 0.65, "HOUSEPLANT");
const WaterPreference WaterPreference::MEDITERRANEAN(0.40, 0.80, "MEDITERRANEAN");
const WaterPreference WaterPreference::VEGETABLE(0.35, 0.70, "VEGETABLE");

const WaterPreference* WaterPreference::allTypes[] = {
    &WaterPreference::SUCCULENT,
    &WaterPreference::FERN,
    &WaterPreference::HOUSEPLANT,
    &WaterPreference::MEDITERRANEAN,
    &WaterPreference::VEGETABLE
};

Plant::Plant(RS485Server& server, int id, String name, PlantState state,
             WaterPreference waterPreference, int saucerCapacity)
    : server(server),
      id(id),
      name(name),
      state(state),
      waterPreference(waterPreference),
      saucerCapacity(saucerCapacity) {}

int Plant::getId() const { return id; }

String Plant::getName() const { return name; }

bool Plant::ping() {
  server.transmitTo(id, "PING");
  String response = server.receiveFrom(id);  // Clear any previous messages
  return response.equals("PONG");
}

bool Plant::loadStatus() {
  server.transmitTo(id, "STATUS");
  String response = server.receiveFrom(id);
  if (response.equals("")) return false;  // No response or error

  response.trim();
  int spaceIndex = response.indexOf(' ');
  if (spaceIndex == -1) return false;  // Invalid response format

  // Split response into parts
  String humidityStr = response.substring(0, spaceIndex);
  String saucerFullStr = response.substring(spaceIndex + 1);
  humidity = humidityStr.toFloat();
  saucerFull = saucerFullStr.toInt() == 1;
  return true;
}

float Plant::getHumidity() {
  return humidity;  // Return the humidity value
}

bool Plant::isSaucerFull() {
  return saucerFull;  // Return the saucer status
}

int Plant::checkWaterNeeds(float humidity) {
  if (saucerFull) return 0;
  if (state == ABSORBING) {
    if (humidity <= waterPreference.minHumidity) {
      return 0;
    }
    state = WATERING; // PLANT NEEDS WATERING

  } else if (state == WATERING) {
    if (humidity >= waterPreference.optimalHumidity) {
      state = ABSORBING;  // STOP WATERING
      return 0; 
    }
  }
  
  return saucerCapacity * 2 * (waterPreference.optimalHumidity - humidity);
}

bool Plant::water(int seconds) {
  loadStatus();
  if (isSaucerFull()) {
    Serial.println("Saucer is full, cannot water.");
    return false;  // Cannot water if saucer is full
  }

  if (seconds <= 0) return false;  // Invalid command
  server.transmitTo(id, "WATER " + String(seconds));
  String response =
      server.receiveFrom(id, true);  // Clear any previous messages
  return response.equals("DONE");
}


