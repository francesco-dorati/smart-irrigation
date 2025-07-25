#include <Arduino.h>
#include <SoftwareSerial.h>

#include "DHT.h"

#include "Plant.h"
#include "RS485Server.h"


const int COMM_PIN = 3;
const int DHT_PIN = 4;  // Pin for DHT sensor
const int RX_PIN = 10;  // RX pin for RS485
const int TX_PIN = 11;  // TX pin for RS485

const int MAX_PLANTS = 8;  // Maximum number of plants


DHT tempSensor(DHT_PIN, DHT11);
RS485Server rs485(COMM_PIN, RX_PIN, TX_PIN);
Plant* plants[MAX_PLANTS] = {nullptr};

void setup() {
  Serial.begin(9600);
  rs485.begin(9600);
  tempSensor.begin();

  /* remove */ plants[0] = new Plant(rs485, 0, "Test Plant",
                                    WaterPreference::MEDITERRANEAN, 100);
  
  Serial.print("\n\nSTART\n");
  status();
  help();
  Serial.print("\n> ");
}

void loop() {
  // Serial command
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase(); 
    if (!command.equals("")) {
      interpretUserCommand(command);
    }
  }
}

void interpretUserCommand(String command) {
  Serial.println();
  String a0 = getArg(command, 0);
  if (a0.equals("PLANT") || a0.equals("P")) {

    String a1 = getArg(command, 1);
    if (a1.equals("NEW") || a1.equals("N")) {  // PLANT NEW <id> <name>
      String id = getArg(command, 2);
      String name = getArg(command, 3);
      newPlant(id, name);

    } else if (a1.equals("LIST") || a1.equals("L")) {  // PLANT LIST
      listPlants();

    } else if (a1.equals("STATUS") || a1.equals("STATUS")) {  // PLANT STATUS <id> 
      String plantId = getArg(command, 2);
      statusPlant(plantId);

    } else if (a1.equals("INFO") || a1.equals("I")) {  // PLANT INFO <id>
      String plantId = getArg(command, 2);
      infoPlant(plantId);

    } else if (a1.equals("WATER") || a1.equals("W")) {  // PLANT WATER <id>
      String plantId = getArg(command, 2);
      waterPlant(plantId);

    } else {
      help();
    }

  } else if (a0.equals("STATUS") || a0.equals("S")) {
    status();

  } else if (a0.equals("INFO") || a0.equals("I")) {
    // local info (battery, uptime, etc.)
    return;

  } else {
    help();
  }

  Serial.print("\n> ");
}

String getArg(String command, int argIndex) {
  for (int i = 0; i <= argIndex; i++) {
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1) {
      if (i == argIndex)
        return command;  // Last argument
      else
        return "";  // Not enough arguments
    }
    if (i == argIndex) {
      return command.substring(0, spaceIndex);
    }
    command = command.substring(spaceIndex + 1);
  }
  return "";
}

Plant* getPlantById(String id) {
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr && plants[i]->getId().equals(id)) {
      return plants[i];
    }
  }
  return nullptr;  // Plant not found
}

// SERIAL DEBUG COMMANDS
String environmentDataString() {
  float humidity = tempSensor.readHumidity();
  float temperature = tempSensor.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    return "Failed to read from DHT sensor!";
  }

  return "Temperature: " + String((int) temperature) + "°C\nAir Humidity: " + String((int) humidity) + "%";
}

String plantStatusString(Plant* plant) {
      bool status = plants[i]->loadStatus();

      String humidity = String((int) (plants[i]->getHumidity()*100));
      String saucerFull = plants[i]->isSaucerFull() ? "FULL" : "EMPTY";
      int waterNeeded = plants[i]->checkWaterNeeds(plants[i]->getHumidity());

      Serial.println("\t- [" + plants[i]->getId() + "] " + plants[i]->getName());
      if (status) {
        Serial.println("\t\tHumidity: \t" + humidity + "%");
        Serial.println("\t\tSaucer: \t" + saucerFull);
        Serial.println("\t\tNeeds Water: \t" + ((waterNeeded > 0) ? "YES, " + String(waterNeeded) + " %" : "NO"));
      } else {
        Serial.println("\t\tCurrently OFFLINE.");
      }
}

bool newPlant() {}

void listPlants() {
  Serial.println("\nYOUR PLANTS:");
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr) {
      String name = plants[i]->getName();
      String id = plants[i]->getId();
      Serial.println("\t- " + name + " [ID: " + id + "]");
    }
  }
}

void statusPlant(String id) {
  Plant* plant = getPlantById(id);
  if (plant == nullptr) {
    Serial.println("Plant " + id + " not found");
    return;
  }
  String statusString = plantStatusString(plant);
  Serial.println(statusString);
}

void infoPlant(String id) {
  // local info (humidity preferences, etc.)
}

void waterPlant(String id) {
  Plant* plant = getPlantById(id);
  if (plant == nullptr) {
    Serial.println("Plant " + id + " not found");
    return;
  }
  Serial.println("Watering " + plant->getName() + "...");
  bool ok = plant->water(5);
  if (ok) {
    Serial.println("Watering completed successfully");
  } else {
    Serial.println("Failed to water " + plant->getName());
  }
}

void status() {
  Serial.println("\n--- STATUS ---\n");

  Serial.println(environmentDataString()+"\n");
  Serial.println("YOUR PLANTS:");
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr) {
      String statusString = plantStatusString(plants[i]);
      Serial.println(statusString);
    }
  }
}

void help() {
  Serial.println("\n--- AVAILABLE COMMANDS ---\n");
  Serial.println("\tINFO - Show local information (battery, uptime, etc.)");
  Serial.println("\tSTATUS - Show the status of all plants");
  Serial.println("");
  Serial.println("\tPLANT LIST - List all plants");
  Serial.println("\tPLANT NEW <id> <name> - Create a new plant");
  Serial.println("\tPLANT REMOVE <id> - Remove plant with the given id");
  Serial.println("\tPLANT PING <id> - Ping a specific plant");
  Serial.println("\tPLANT INFO <id> - Get information about a specific plant");
  Serial.println(
      "\tPLANT CHECK <id> - Check the status of a specific plant and decide "
      "whether to water");
}