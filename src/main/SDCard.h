#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "Plant.h"

/*
PLANT FILE:
id, name, state, waterPreferenceName, minHumidity, optimalHumidity, saucerCapacity

*/


const String SD_CARD_TEST_FILE = "/test.txt";
const String SD_CARD_PLANTS_FILE = "/plants.csv";

class SDCard {
public:
    SDCard(gpio_num_t CS_PIN, 
        gpio_num_t SCK_PIN, 
        gpio_num_t MISO_PIN, 
        gpio_num_t MOSI_PIN);
    bool start();
    void stop();

    int getTotalSpace();
    int getFreeSpace();

    bool testWrite();
    String testRead();

    void addPlant(Plant *plant);
    bool loadPlants(Plant *plants[], int maxPlants, RS485Server& rs485);

    void logWakeUp();
    void logSleep(int uptime);
    void logPlantsOn();
    void logPlantsOff();
    void logPlantStatus(int id, String status);
    void logPlantWater(int id, int amount);

private:
    gpio_num_t csPin;
    gpio_num_t sckPin;
    gpio_num_t misoPin;
    gpio_num_t mosiPin;
};

#endif
