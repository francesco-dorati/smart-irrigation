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

  /* remove */ plants[0] = new Plant(rs485, "0", "Test Plant",
                                    WaterPreference::MEDITERRANEAN, 5);
  /* remove */ //plants[1] = new Plant(rs485, "1", "Fake Plant");
  
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

    } else if (a1.equals("PING") || a1.equals("P")) {  // PLANT PING <id> 
      String plantId = getArg(command, 2);
      pingPlant(plantId);

    } else if (a1.equals("INFO") || a1.equals("I")) {  // PLANT INFO <id>
      String plantId = getArg(command, 2);
      infoPlant(plantId);

    } else if (a1.equals("CHECK") || a1.equals("C")) {  // PLANT CHECK <id>
      String plantId = getArg(command, 2);
      checkPlant(plantId);

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

bool newPlant(String id, String name) {}

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

void pingPlant(String id) {
  Plant* plant = getPlantById(id);
  if (plant == nullptr) {
    Serial.println("Plant " + id + " not found");
    return;
  }
  Serial.println(plant->getName() + " is " + (plant->ping() ? "working correctly!" : "OFFLINE"));
}

void infoPlant(String id) {
  // local info (humidity preferences, etc.)
}

void checkPlant(String id) {
  // checks humidity level and decides whether to water
  Serial.println("\n--- CHECK ---\n");

  Plant* plant = getPlantById(id);
  if (plant == nullptr) {
    Serial.println("Plant " + id + " not found");
    return;
  }
  Serial.println("\nChecking " + plant->getName() + "...");

  // PING
  bool status = plant->loadStatus();
  if (!status) {
    Serial.println(plant->getName() + " is OFFLINE.");
    return;
  }

  // HUMIDITY
  int humidity = plant->getHumidity()*100;
  bool saucerFull = plant->isSaucerFull();
  Serial.println("\nHumidity: " + String(humidity) + "%");
  Serial.println("Saucer full: " + String(saucerFull ? "YES" : "NO"));

  // WATER NEEDS
  int waterNeeds = plant->getWaterNeeds(humidity);
  if (waterNeeds > 0) {
    Serial.println("\nPlant needs watering!");
    char confirmation;
    do {
      Serial.print("Do you want me to water it? (y/n)  ");
      while (!Serial.available()) {}
      confirmation = Serial.read();
      confirmation = tolower(confirmation);
    } while (confirmation != 'y' && confirmation != 'n');

    if (confirmation == 'y') {
      Serial.println("Watering " + plant->getName() + " for " + String(waterNeeds) + " seconds... ");
      plant->water(waterNeeds);
      Serial.println("Done watering!");
    } else {
      Serial.println("\nOkay, not watering.");
    }

  } else {
    Serial.println("\nNo need to water.");
    // LAST WATERED
    String lastWatered = plant->getLastWatered();
    Serial.println("\nLast watered: " + lastWatered);
  }

  delay(800);  
  Serial.println("\n--- END OF CHECK ---\n");

}

void status() {
  Serial.println("\n--- STATUS ---\n");

  Serial.println(environmentDataString()+"\n");
  Serial.println("YOUR PLANTS:");
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr) {
      String name = plants[i]->getName();
      String id = plants[i]->getId();
      bool status = plants[i]->loadStatus();
      String humidity = String((int) (plants[i]->getHumidity()*100));
      String lastWatered = plants[i]->getLastWatered();
      String saucerFull = plants[i]->isSaucerFull() ? "FULL" : "EMPTY";

      Serial.println("\t- [" + id + "] " + name);
      if (status) {
        Serial.println("\t\tHumidity: \t" + humidity + "%");
        Serial.println("\t\tSaucer: \t" + saucerFull);
        Serial.println("\t\tLast watered: \t" + lastWatered);
      } else {
        Serial.println("\t\tCurrently OFFLINE.");
      }
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