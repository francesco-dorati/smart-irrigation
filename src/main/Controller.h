#ifndef POWER_SWITCH_H
#define POWER_SWITCH_H


#include <Arduino.h>
#include <Wire.h>

#include "WiFiManager.h"
#include "DHT.h"
#include "Plant.h"
#include "RS485Server.h"
#include "RTClib.h"
#include "driver/rtc_io.h"
#include "SDCard.h"

const gpio_num_t POWER_PIN = (gpio_num_t) 2;  // Pin to control power to the plants
const gpio_num_t SHIFTER_POWER_PIN = (gpio_num_t) 5;
const gpio_num_t DHT_PIN = (gpio_num_t) 15;  // Pin for DHT sensor

const gpio_num_t COMM_PIN = (gpio_num_t) 4; // Pin for RS485 communication 
const gpio_num_t RX_PIN = (gpio_num_t) 16;  // RX pin for RS485
const gpio_num_t TX_PIN = (gpio_num_t) 17;  // TX pin for RS485

const gpio_num_t SD_CARD_SCK_PIN = (gpio_num_t) 18; // SCK pin for SD card
const gpio_num_t SD_CARD_MISO_PIN = (gpio_num_t) 19; // MISO pin for SD card
const gpio_num_t SD_CARD_MOSI_PIN = (gpio_num_t) 23; // MOSI pin for SD card
const gpio_num_t SD_CARD_CS_PIN = (gpio_num_t) 21;  // Chip select pin for SD card

const gpio_num_t BUTTON_WAKEUP_PIN = (gpio_num_t) 32;
const gpio_num_t RTC_WAKEUP_PIN = (gpio_num_t) 33;

const int MAX_PLANTS = 8;  // Maximum number of plants

class Controller {
 public:
  Controller(WiFiManager& wifi);
  void init();

  bool isOn();
  void turnOn();
  void turnOff();
  void sleep(int seconds);

  bool addPlant(int id, String name, WaterPreference waterPreference, int saucerCapacity);
  Plant* getPlant(int id);
  Plant** getPlants();

  SDCard& getSDCard();

  String getTimeString();
  String getReadableTimeString();
  String getEnvironmentDataString();
  String getPlantStatusString(int id);

 private:
  WiFiManager& wifi;  // Reference to WiFiManager for printing messages
  RTC_DS3231 rtc;
  DHT tempSensor;
  RS485Server rs485;
  SDCard sdCard;
  Plant* plants[MAX_PLANTS] = {nullptr};  // Array to hold plant objects

  bool isPowerOn;  // true if ON, false if OFF
};

#endif  // POWER_SWITCH_H