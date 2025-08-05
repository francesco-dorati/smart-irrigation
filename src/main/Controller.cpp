#include "Controller.h"

Controller::Controller() : isPowerOn(false), tempSensor(DHT_PIN, DHT11), rs485(COMM_PIN, RX_PIN, TX_PIN, 9600) {}

void Controller::init() {
  pinMode(POWER_PIN, INPUT);  // Set pin to INPUT to avoid floating state
  //rs485.begin(9600);
  //tempSensor.begin();

  /* remove */ plants[0] = new Plant(rs485, 0, "Test Plant", PlantState::ABSORBING,
                                    WaterPreference::MEDITERRANEAN, 100);
  
}

Plant* Controller::getPlant(int id) {
  turnOn();
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr && plants[i]->getId() == id) {
      return plants[i];
    }
  }
  return nullptr;  // Plant not found
}

Plant** Controller::getPlants() {
  return plants;  // Return the array of plant pointers
}

bool Controller::isOn() { return isPowerOn; }

void Controller::turnOn() {
  pinMode(POWER_PIN, OUTPUT);  // Set pin to OUTPUT to control power
  digitalWrite(POWER_PIN, HIGH);
    // Allow some time for the power to turn on
  if (!isPowerOn) {
    // Powering on the system
    delay(5000);  // Allow some time for the power to stabilize
    rs485.start();
    tempSensor.begin();
    delay(5000);
  }
  isPowerOn = true;
}

void Controller::turnOff() {
  digitalWrite(POWER_PIN, LOW);
  delay(100);  // Allow some time for the power to turn off
  //pinMode(POWER_PIN, INPUT);  // Set pin back to INPUT to avoid floating state
  if (isPowerOn) {
    rs485.stop();
  }
  isPowerOn = false;
}

String Controller::getEnvironmentDataString() {
  turnOn(); 
  float humidity = tempSensor.readHumidity();
  float temperature = tempSensor.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    // Retry once
    tempSensor.begin();
    delay(5000);
    humidity = tempSensor.readHumidity();
    temperature = tempSensor.readTemperature();
    Serial.println("Retrying DHT sensor read...");
  }

  if (isnan(humidity) || isnan(temperature)) {
    return "Failed to read from DHT sensor!";
  }

  return "Temperature: " + String((int) temperature) + "°C\nAir Humidity: " + String((int) humidity) + "%";
}

String Controller::getPlantStatusString(int id) {
      turnOn(); 
      String s = "";
      Plant* plant = getPlant(id);
      if (plant == nullptr) {
          s += "\n\t\tCurrently OFFLINE.";
          //turnOff();
          return s;
      }

      bool status = plant->loadStatus();

      String humidity = String((int) (plant->getHumidity()*100));
      String saucerFull = plant->isSaucerFull() ? "FULL" : "EMPTY";
      int waterNeeded = plant->checkWaterNeeds(plant->getHumidity());

      s += "\t- [" + String(plant->getId()) + "] " + plant->getName();
      if (status) {
        s += "\n\t\tHumidity: \t" + humidity + "%";
        s += "\n\t\tSaucer: \t" + saucerFull;
        s += "\n\t\tNeeds Water: \t" + ((waterNeeded > 0) ? "YES, " + String(waterNeeded) + " %" : "NO");
      } else {
        s += "\n\t\tCurrently OFFLINE.";
      }

      //turnOff();
      return s;
}

