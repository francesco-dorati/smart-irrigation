#ifndef POWER_SWITCH_H
#define POWER_SWITCH_H


#include <Arduino.h>
#include <Wire.h>

#include "DHT.h"
#include "Plant.h"
#include "RS485Server.h"
#include "RTClib.h"
#include "driver/rtc_io.h"

const gpio_num_t POWER_PIN = (gpio_num_t) 2;  // Pin to control power to the plants
const gpio_num_t COMM_PIN = (gpio_num_t) 4;
const gpio_num_t SHIFTER_POWER_PIN = (gpio_num_t) 5;
const gpio_num_t DHT_PIN = (gpio_num_t) 15;  // Pin for DHT sensor
const gpio_num_t RX_PIN = (gpio_num_t) 16;  // RX pin for RS485
const gpio_num_t TX_PIN = (gpio_num_t) 17;  // TX pin for RS485
const gpio_num_t BUTTON_WAKEUP_PIN = (gpio_num_t) 32;
const gpio_num_t RTC_WAKEUP_PIN = (gpio_num_t) 33;

const int MAX_PLANTS = 8;  // Maximum number of plants

class Controller {
 public:
  Controller();
  void init();

  bool isOn();
  void turnOn();
  void turnOff();
  void sleep(int seconds);

  Plant* getPlant(int id);
  Plant** getPlants();

  String getTimeString();
  String getReadableTimeString();
  String getEnvironmentDataString();
  String getPlantStatusString(int id);

 private:
  RTC_DS3231 rtc;
  DHT tempSensor;
  RS485Server rs485;
  Plant* plants[MAX_PLANTS] = {nullptr};  // Array to hold plant objects

  bool isPowerOn;  // true if ON, false if OFF
};

#endif  // POWER_SWITCH_H