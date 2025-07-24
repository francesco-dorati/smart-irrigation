#include "Plant.h"

const WaterPreference WaterPreference::SUCCULENT(15, 60);
const WaterPreference WaterPreference::FERN(55, 85);
const WaterPreference WaterPreference::HOUSEPLANT(35, 75);
const WaterPreference WaterPreference::VEGETABLE(45, 80);
const WaterPreference WaterPreference::MEDITERRANEAN(40, 80);

Plant::Plant(RS485Server& server, int id, String name, PlantState state,
             WaterPreference waterPreference, int saucerCapacity)
    : server(server),
      id(id),
      name(name),
      state(state),
      lastWatered(lastWatered),
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
  if (saucerFull) {
    Serial.println("Saucer is full, cannot water.");
    return false;  // Cannot water if saucer is full
  }

  if (seconds <= 0) return false;  // Invalid command
  server.transmitTo(id, "WATER " + String(seconds));
  String response =
      server.receiveFrom(id, true);  // Clear any previous messages
  return response.equals("DONE");
}


