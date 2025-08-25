#include <Arduino.h>
#include <ArduinoOTA.h>

#include "Controller.h"
#include "WiFiManager.h"

WiFiManager wifi("iPhone di Francesco", "grinder420");
Controller controller(wifi);

void setup() {
  Serial.begin(115200);
  wifi.begin();
  delay(2000);
  controller.init();
  delay(1000);

  ArduinoOTA.setHostname("smart-irrigation"); // Optional
  ArduinoOTA.begin();

  wifi.println("\n\n\n--- SMART IRRIGATION ---\n");
  status();
  help();
  prompt();
}

void loop() {
  ArduinoOTA.handle();

  // Serial command
  if (Serial.available()) {
    String command = readCommand();
    if (!command.equals("")) {
      interpretUserCommand(command);
    }
  }

  if (wifi.available()) {
    String command = wifi.getCommand();
    if (!command.equals("")) {
      interpretUserCommand(command);
    }
  }
}

String readCommand() {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase(); 
    return command;
}

void interpretUserCommand(String command) {
  wifi.println("");
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
    info();

  } else if (a0.equals("ON") || a0.equals("1")) {
    wifi.println("Powering ON");
    controller.turnOn();

  } else if (a0.equals("OFF") || a0.equals("0")) {
    wifi.println("Powering OFF");
    controller.turnOff();

  } else if (a0.equals("SLEEP") || a0.equals("Z")) {
    int sleepTime = getArg(command, 1).toInt();
    if (sleepTime <= 0) {
      wifi.println("Invalid sleep time. Please specify a positive number.");
      return;
    }
    controller.sleep(sleepTime);

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
void newPlant() {
  wifi.println("\n\n--- PLANT CREATION ---\n");

  // ID
  int plantId;
  wifi.println("\nPlease enter the plant ID: ");
  while (!wifi.available());
  String plantIdString = wifi.getCommand();
  plantId = plantIdString.toInt();

  // NAME
  String plantName;
  wifi.println("\nPlease enter the plant name: ");
  while (!wifi.available());
  plantName = wifi.getCommand();

  // TYPE (WATER PREFERENCE)
  WaterPreference waterPreference;
  int typeCount = sizeof(WaterPreference::allTypes) / sizeof(WaterPreference*);
  int typeSelection;
  do {
    wifi.println("\nPlease choose the plant type:");
    for (int i = 1; i <= typeCount; i++) {
      const WaterPreference* type = WaterPreference::allTypes[i-1];
      wifi.println("\t[" + String(i) + "] " + type->toString() + 
                   " (Min: " + String((int)type->minHumidity) + "%, Optimal: " + String((int)type->optimalHumidity) + "%)");
    }
    wifi.println("\t[0] CUSTOM");

    wifi.println("\nEnter the number corresponding to the plant type: ");
    while (!wifi.available());
    String typeSelectionString = wifi.getCommand();
    typeSelection = typeSelectionString.toInt();
  } while (typeSelection < 0 || typeSelection > typeCount);

  if (typeSelection == 0) {
    // CUSTOM type selected
    // min humidity
    wifi.println("\nPlease enter the minimum humidity value (in %): ");
    while (!wifi.available());
    String minHumidityString = wifi.getCommand();
    float minHumidity = minHumidityString.toFloat();
    // optimal humidity
    wifi.println("\nPlease enter the optimal humidity value (in %): ");
    while (!wifi.available());
    String optimalHumidityString = wifi.getCommand();
    float optimalHumidity = optimalHumidityString.toFloat();
    // preference creation
    waterPreference = WaterPreference(minHumidity, optimalHumidity, "CUSTOM");
  } else {
    // preference selection
    waterPreference = *WaterPreference::allTypes[typeSelection - 1];
  }

  // SAUCER CAPACITY
  int saucerCapacity;
  // @TODO SAUCER CAPACITY TEST
  wifi.println("\nPlease enter the saucer capacity (in ml): ");
  while (!wifi.available());
  String saucerCapacityString = wifi.getCommand();
  saucerCapacity = saucerCapacityString.toInt();

  // PLANT OVERVIEW
  wifi.println("\n--- PLANT OVERVIEW ---\n");
  wifi.println("ID: " + String(plantId));
  wifi.println("Name: " + plantName);
  wifi.println("Type: " + waterPreference.toString() + 
               " (Min: " + String((int)waterPreference.minHumidity) + "%, Optimal: " + String((int)waterPreference.optimalHumidity) + "%)");
  wifi.println("Saucer Capacity: " + String(saucerCapacity) + " ml");

  // CONFIRMATION
  wifi.println("\nDo you want to proceed with the creation? (y/n)");
  while (!wifi.available());
  String confirmation = wifi.getCommand();
  confirmation.toLowerCase();
  if (!confirmation.equals("y") && !confirmation.equals("yes")) {
    wifi.println("\nPlant creation cancelled.");
    return;
  }

  // PLANT CREATION
  bool ok = controller.addPlant(plantId, plantName, waterPreference, saucerCapacity);
  if (ok) {
    wifi.println("\nPlant created successfully!");
  } else {
    wifi.println("\nFailed to create plant.");
  }
}

void info() {
  wifi.println("\n--- SYSTEM INFO ---\n");
  wifi.println("SD Card: " + String(controller.getSDCard().getFreeSpace()) + " GB free (" + String(controller.getSDCard().getTotalSpace()) + " GB total)");
  wifi.println("Current time: " + controller.getReadableTimeString());
  wifi.println("Battery: -- %");
  wifi.println("Uptime: -- min");
  wifi.println("\nYOUR PLANTS:");
  Plant** plants = controller.getPlants();
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr) {
      String infoString = controller.getPlantInfoString(plants[i]->getId());
      wifi.println(infoString);
    }
  }
}

void statusPlant(int id) {
  String statusString = controller.getPlantStatusString(id);
  wifi.println(statusString);
}

void infoPlant(int id) {
  String infoString = controller.getPlantInfoString(id);
  wifi.println(infoString);
}

void waterPlant(int id) {
  Plant* plant = controller.getPlant(id);
  if (plant == nullptr) {
    wifi.println("Plant " + String(id) + " not found");
    return;
  }

  wifi.println("Watering " + plant->getName() + "...");
  bool ok = plant->water(30);
  if (ok) {
    wifi.println("Watering completed successfully");
  } else {
    wifi.println("Failed to water " + plant->getName());
  }
}

void status() {
  wifi.println("\n--- STATUS ---\n");
  wifi.println(controller.getEnvironmentDataString()+"\n");
  wifi.println("YOUR PLANTS:");
  Plant** plants = controller.getPlants();
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr) {
      String statusString = controller.getPlantStatusString(plants[i]->getId());
      wifi.println(statusString);
    }
  }
}

void help() {
  wifi.println("\n--- AVAILABLE COMMANDS ---\n");
  wifi.println("\tINFO - Show local information (battery, uptime, etc.)");
  wifi.println("\tON/OFF - Turn on/off the system");
  wifi.println("\tSTATUS - Show the status of all plants");
  wifi.println("\tSLEEP <sec> - Put the system to sleep for a specified number of seconds");
  //Serial.println("");
  wifi.println("\tPLANT LIST - List all plants");
  wifi.println("\tPLANT NEW - Create a new plant");
  wifi.println("\tPLANT REMOVE <id> - Remove plant with the given id");
  wifi.println("\tPLANT INFO <id> - Get information about a specific plant");
  wifi.println("\tPLANT STATUS <id> - Get the status of a specific plant");
  wifi.println("\tPLANT WATER <id> - Water a specific plant");
}

void prompt() {  
  String s = "\n";
  s += controller.getReadableTimeString();
  
  s += " | Power ";
  s += controller.isOn() ? "ON" : "OFF";
  
  s += " | WiFi ";
  s += wifi.isConnected() ? "CONNECTED" : "DISCONNECTED";

  s += "\n> ";

  wifi.print(s);

}