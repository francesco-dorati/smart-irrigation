#include "RS485Client.h"

RS485Client::RS485Client(String id, int comm, int rx, int tx)
    : id(id), comm(comm), serial(rx, tx) {}

void RS485Client::begin(long baudrate) {
  serial.begin(baudrate);
  pinMode(comm, OUTPUT);
  digitalWrite(comm, LOW);
}

bool RS485Client::available() { return serial.available(); }

String RS485Client::receiveCommand() {  // Ensure command is from this plant
  String command = "";
  unsigned long startTime = millis();
  bool timeout = true;
  
  // Read with timeout to handle incomplete messages
  while (millis() - startTime < TIMEOUT) { // 100ms timeout
    if (serial.available()) {
      char c = serial.read();
      if (c == '\n' || c == '\r') {
        if (command.length() > 0) {
          timeout = false;
          break;
        }
      } else {
        command += c;
      }
      startTime = millis(); // Reset timeout on new character
    }
  }

  if (timeout) {
    Serial.println("ERROR: timeout, received: \"" + command + "\"");
    return "";  // Timeout, no response
  }

  command.trim();
  String recipient = getRecipient(command);
  if (recipient.equals(id) || recipient.equals("*")) {
    command = getBody(command);
  } else {
    command = "";  // Not for this target
  }
  
  return command;
}

void RS485Client::transmit(String message) {
  delay(10);
  digitalWrite(comm, HIGH);
  delayMicroseconds(100);

  serial.println((id + " " + message).c_str());
  serial.flush();
  delayMicroseconds(100);

  digitalWrite(comm, LOW);
  delayMicroseconds(100);
  
  Serial.println("sent: " + message);
}

String RS485Client::getRecipient(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) return "";
  return command.substring(0, spaceIndex);
}

String RS485Client::getBody(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) return "";
  return command.substring(spaceIndex + 1);
}
