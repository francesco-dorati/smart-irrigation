#include <Arduino.h>

#include "HumiditySensor.h"
#include "LED.h"
#include "Pump.h"
#include "RS485Client.h"
#include "SaucerSensor.h"

const int PLANT_ID = 0;  // Unique ID for this plant

const int SENSOR_PIN = A0;
const int LED_PIN = 2;
const int COMM_PIN = 3;  // LOW: listening, HIGH: sending
const int PUMP_PIN = 4;  // LOW: ON, HIGH: OFF
const int SAUCER_POWER_PIN = 5; // OUTPUT LOW: POWER OFF, HIGH: POWER ON
const int SAUCER_READING_PIN = 6; // INPUT LOW: EMPTY, HIGH: FULL
const int RX_PIN = 11;   // RX pin for RS485
const int TX_PIN = 10;   // TX pin for RS485

RS485Client rs485(PLANT_ID, COMM_PIN, RX_PIN, TX_PIN);
LED led(LED_PIN);
HumiditySensor humiditySensor(SENSOR_PIN);
SaucerSensor saucerSensor(SAUCER_POWER_PIN, SAUCER_READING_PIN);
Pump pump(PUMP_PIN);

void setup() {
  Serial.begin(9600);

  rs485.begin(9600);
  led.begin();
  humiditySensor.begin();
  saucerSensor.begin();
  pump.begin();

  led.blink();
  led.blink();
  Serial.println("START");
  delay(1000);
}

void loop() {
  // RS485 communication
  if (rs485.available()) {
    // Read command from RS485
    String command = rs485.receiveCommand();
    if (!command.equals("")) {
      interpretCommand(command);
    }
  }

  // Serial communication (DEBUG)
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    Serial.println("debugging: \"" + command + "\"");
    interpretDebug(command);
  }
}

void interpretCommand(String command) {

  if (command.equals("PING")) {
    led.blink();
    rs485.transmit("PONG");

  } else if (command.startsWith("WATER")) {
    int seconds = command.substring(6).toInt();
    if (seconds < 0) return;  // Invalid command
    pump.activate(seconds);
    rs485.transmit("DONE");

  } else if (command.startsWith("STATUS")) { 
    float humidity = humiditySensor.getHumidity();
    bool full = saucerSensor.isFull();
    String status = String(humidity) + " " + (full ? "1" : "0");
    rs485.transmit(status);

  } else {
    Serial.println("Unknown command: \"" + command + "\"");
  }
}

void interpretDebug(String command) {
  if (command.equals("PING")) {
    led.blink();
    Serial.println("PONG");
  } else if (command.equals("HUMIDITY")) {
    int value = humiditySensor.getValue();
    float voltage = humiditySensor.getVoltage();
    int humidity = humiditySensor.getHumidity();
    Serial.println("Humidity: " + String(humidity) + "%, Voltage: " +
                   String(voltage) + "V, value: " + String(value));
  } else if (command.startsWith("WATER")) {
    int seconds = command.substring(6).toInt();
    if (seconds < 0) return;  // Invalid command
    pump.activate(seconds);
    Serial.println("Watering DONE.");
  } else {
    Serial.println("Unknown command: " + command);
  }
}
