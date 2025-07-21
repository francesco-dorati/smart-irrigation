#include "RS485Server.h"

RS485Server::RS485Server(int comm, int rx, int tx)
    : comm(comm), serial(rx, tx) {}

void RS485Server::begin(long baudrate) {
  serial.begin(baudrate);
  pinMode(comm, OUTPUT);
  digitalWrite(comm, LOW);
}

/*
bool RS485Server::available() {
  while (serial.available()) {
    char c = serial.read();
    lineBuffer += c;
    if (c == '\n') {
      return true;  // We have a full line!
    }
  }
  return false;  // Not a full line yet}
}
*/
/*String RS485Server::receiveResponse() {
  String command = serial.readStringUntil('\n');
  command.trim();
  return command;
}*/

String RS485Server::receiveFrom(String target, bool avoidTimeout = false) {
  String response = "";
  unsigned long startTime = millis();
  bool timeout = true;

  while (millis() - startTime < TIMEOUT || avoidTimeout) {
    if (serial.available()) {
      char c = serial.read();
      if (c == '\n' || c == '\r') {
        if (response.length() > 0) {
          timeout = false;
          //Serial.println("Received: " + response);
          break; // End of message
        }
      } else {
        response += c;
      }
    }
  }
  
  if (timeout) {
    Serial.println("ERROR: timeout");
    return "";  // Timeout, no response
  }
  
  // Check if the response is for the target
  response.trim();
  String recipient = getRecipient(response);
  if (recipient.equals(target) || recipient.equals("*")) {
    response = getBody(response);
  } else {
    Serial.println("ERROR: Received message not for this target: " + recipient);
    response = "";  // Not for this target
  }
  response.trim();
  return response;  // Convert to uppercase for consistency
}

void RS485Server::transmitTo(String target, String message) {
  delay(200);
  digitalWrite(comm, HIGH);
  delayMicroseconds(100);

  serial.println((target + " " + message).c_str());
  serial.flush();
  delayMicroseconds(100);

  digitalWrite(comm, LOW);
  delayMicroseconds(100);
  //Serial.println("sent: " + message);
}

String RS485Server::getRecipient(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) return "";
  return command.substring(0, spaceIndex);
}

String RS485Server::getBody(String command) {
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) return "";
  return command.substring(spaceIndex + 1);
}
