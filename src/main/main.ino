#include <Arduino.h>
#include <SoftwareSerial.h>

#include "RS485Server.h"

const int COMM_PIN = 2;
const int RX_PIN = 10;  // RX pin for RS485
const int TX_PIN = 11;  // TX pin for RS485

RS485Server rs485(COMM_PIN, RX_PIN, TX_PIN);

void setup() {
  Serial.begin(9600);
  rs485.begin(9600);
  Serial.println("START\n");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    rs485.transmitTo("*", command);
  }

  if (rs485.available()) {
    String response = rs485.receiveResponse();
    Serial.println("received: " + getBody(response) + "\n");
  }
}

String getRecipient(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) return "";
  return command.substring(0, spaceIndex);
}

String getBody(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) return "";
  return command.substring(spaceIndex + 1);
}