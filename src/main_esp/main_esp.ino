#include <Arduino.h>

const int SWITCH_PIN = 2;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\nSTARTED!\n\n");
  pinMode(SWITCH_PIN, INPUT);
  digitalWrite(SWITCH_PIN, LOW);
  pinMode(SWITCH_PIN, INPUT);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    Serial.println("debugging: \"" + command + "\"");
    interpretCommand(command);
  }
}

void interpretCommand(String command) {
  if (command.equals("ON")) {
    pinMode(SWITCH_PIN, OUTPUT);
    digitalWrite(SWITCH_PIN, HIGH);
    Serial.println("Switch ON");

  } else if (command.equals("OFF")) {
    digitalWrite(SWITCH_PIN, LOW);
    pinMode(SWITCH_PIN, INPUT);
    Serial.println("Switch OFF");

  } else {
    Serial.println("Unknown command: \"" + command + "\"");
  }
}