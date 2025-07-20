#include "Plant.h"

Plant::Plant(RS485Server& server, String id, String name, String lastWatered,
             WaterPreference waterPreference, SunlightExposure exposure)
    : server(server),
      id(id),
      name(name),
      lastWatered(lastWatered),
      waterPreference(waterPreference),
      exposure(exposure) {}

String Plant::getId() const { return id; }

String Plant::getName() const { return name; }

String Plant::getLastWatered() { return lastWatered; }

bool Plant::ping() {
  server.transmitTo(id, "PING");
  String response = server.receiveFrom(id);  // Clear any previous messages
  return response.equals("PONG");
}

int Plant::getHumidity() {
  server.transmitTo(id, "HUMIDITY");
  String response = server.receiveFrom(id);  // Clear any previous messages
  if (response.equals("")) return -1;        // No response or error
  return response.toInt();
}

bool Plant::water(int seconds) {
  if (seconds <= 0) return;  // Invalid command
  server.transmitTo(id, "WATER " + String(seconds));
  String response =
      server.receiveFrom(id, true);  // Clear any previous messages
  if (!response.equals("DONE")) {
    Serial.println("Failed to water " + name + ": " + response);
  } else {
    lastWatered = "N/A";  // REPLACE get from RTC clock
  }
}

int Plant::getWaterNeeds(int humidity) {
  // check if last watered is not recent
  if (humidity < 0) return -1;  // Invalid humidity
  if (humidity < 50) return 5;  // Needs watering
  return 0;                     // No need to water
}
