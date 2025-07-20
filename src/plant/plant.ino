#include <Arduino.h>

#include "HumiditySensor.h"
#include "LED.h"
#include "Pump.h"
#include "RS485Client.h"

const String PLANT_ID = "0";  // Unique ID for this plant

const int SENSOR_PIN = A0;
const int LED_PIN = 2;
const int COMM_PIN = 3;  // LOW: listening, HIGH: sending
const int PUMP_PIN = 4;  // LOW: ON, HIGH: OFF
const int RX_PIN = 10;   // RX pin for RS485
const int TX_PIN = 11;   // TX pin for RS485

RS485Client rs485(PLANT_ID, COMM_PIN, RX_PIN, TX_PIN);
LED led(LED_PIN);
HumiditySensor humiditySensor(SENSOR_PIN);
Pump pump(PUMP_PIN);

void setup() {
  Serial.begin(9600);

  rs485.begin(9600);
  led.begin();
  humiditySensor.begin();
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
  } else if (command.equals("HUMIDITY")) {
    int humidity = humiditySensor.getHumidity();
    rs485.transmit(String(humidity));
  } else if (command.startsWith("WATER")) {
    int seconds = command.substring(6).toInt();
    if (seconds < 0) return;  // Invalid command
    pump.activate(seconds);
    rs485.transmit("DONE");
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
  } else {
    Serial.println("Unknown command: " + command);
  }
}
