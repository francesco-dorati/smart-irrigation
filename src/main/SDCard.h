#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include "Plant.h"

class SDCard {
public:
    bool loadPlants(Plant *plants[], int maxPlants);
    // void addPlant(Plant *plant);

    void logWakeUp();
    void logSleep(int uptime);
    void logPlantsOn();
    void logPlantsOff();
    void logPlantStatus(int id, String status);
    void logPlantWater(int id, int amount);

private:

};

#endif
