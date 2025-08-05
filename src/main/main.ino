#include <Arduino.h>

#include "Controller.h"


Controller controller;

void setup() {
  Serial.begin(115200);
  controller.init();
  delay(2000);
  Serial.print("\n\nSTART\n");
  status();
  help();
  prompt();
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
      // ON command (sd)
      newPlant();

    } else if (a1.equals("LIST") || a1.equals("L")) {  // PLANT LIST
      // OFF command
      listPlants();

    } else if (a1.equals("INFO") || a1.equals("I")) {  // PLANT INFO <id>
      // OFF command
      int plantId = getArg(command, 2).toInt();
      infoPlant(plantId);

    } else if (a1.equals("STATUS") || a1.equals("S")) {  // PLANT STATUS <id> 
      int plantId = getArg(command, 2).toInt();
      statusPlant(plantId);

    } else if (a1.equals("WATER") || a1.equals("W")) {  // PLANT WATER <id>
      int plantId = getArg(command, 2).toInt();
      waterPlant(plantId);

    } else {
      help();
    }

  } else if (a0.equals("STATUS") || a0.equals("S")) {
    status();

  } else if (a0.equals("INFO") || a0.equals("I")) {
    // local info (battery, uptime, etc.)
    return;

  } else if (a0.equals("ON") || a0.equals("1")) {
    controller.turnOn();
    Serial.println("Power ON");

  } else if (a0.equals("OFF") || a0.equals("0")) {
    controller.turnOff();
    Serial.println("Power OFF");

  } else {
    help();
  }

  prompt();
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



// SERIAL DEBUG COMMANDS
bool newPlant() {}

void listPlants() {
  Serial.println("\nYOUR PLANTS:");
  Plant** plants = controller.getPlants();
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr) {
      String name = plants[i]->getName();
      int id = plants[i]->getId();
      Serial.println("\t- " + name + " [ID: " + String(id) + "]");
    }
  }
}

void statusPlant(int id) {
  String statusString = controller.getPlantStatusString(id);
  Serial.println(statusString);
}

void infoPlant(int id) {
  // local info (humidity preferences, etc.)
}

void waterPlant(int id) {
  Plant* plant = controller.getPlant(id);
  if (plant == nullptr) {
    Serial.println("Plant " + String(id) + " not found");
    return;
  }

  Serial.println("Watering " + plant->getName() + "...");
  bool ok = plant->water(30);
  if (ok) {
    Serial.println("Watering completed successfully");
  } else {
    Serial.println("Failed to water " + plant->getName());
  }
}

void status() {
  Serial.println("\n--- STATUS ---\n");
  Serial.println(controller.getEnvironmentDataString()+"\n");
  Serial.println("YOUR PLANTS:");
  Plant** plants = controller.getPlants();
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr) {
      String statusString = controller.getPlantStatusString(plants[i]->getId());
      Serial.println(statusString);
    }
  }
}

void help() {
  Serial.println("\n--- AVAILABLE COMMANDS ---\n");
  Serial.println("\tINFO - Show local information (battery, uptime, etc.)");
  Serial.println("\tON - Turn on the system");
  Serial.println("\tOFF - Turn off the system");
  Serial.println("\tSTATUS - Show the status of all plants");
  //Serial.println("");
  Serial.println("\tPLANT LIST - List all plants");
  Serial.println("\tPLANT NEW - Create a new plant");
  Serial.println("\tPLANT REMOVE <id> - Remove plant with the given id");
  Serial.println("\tPLANT INFO <id> - Get information about a specific plant");
  Serial.println("\tPLANT STATUS <id> - Get the status of a specific plant");
  Serial.println(
      "\tPLANT WATER <id> - Water a specific plant");
}

void prompt() {
  String s = "\n";
  s += "Power: ";
  s += controller.isOn() ? "ON" : "OFF";
  s += "\n> ";
  Serial.print(s);
}