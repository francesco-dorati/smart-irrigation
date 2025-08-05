#ifndef POWER_SWITCH_H
#define POWER_SWITCH_H

#include <Arduino.h>
#include <DHT.h>

#include "Plant.h"
#include "RS485Server.h"
// #include "DHT.h"

const int POWER_PIN = 2;  // Pin to control power to the plants
const int COMM_PIN = 4;
const int DHT_PIN = 5;  // Pin for DHT sensor
const int RX_PIN = 16;  // RX pin for RS485
const int TX_PIN = 17;  // TX pin for RS485

const int MAX_PLANTS = 8;  // Maximum number of plants

class Controller {
 public:
  Controller();
  void init();

  bool isOn();
  void turnOn();
  void turnOff();

  Plant* getPlant(int id);
  Plant** getPlants();

  String getEnvironmentDataString();
  String getPlantStatusString(int id);

 private:
  DHT tempSensor;
  RS485Server rs485;
  Plant* plants[MAX_PLANTS] = {nullptr};  // Array to hold plant objects

  bool isPowerOn;  // true if ON, false if OFF
};

#endif  // POWER_SWITCH_H