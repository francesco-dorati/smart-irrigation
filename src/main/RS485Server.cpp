#include "RS485Server.h"

RS485Server::RS485Server(int comm, int rx, int tx, long baudrate)
    : comm(comm), rx(rx), tx(tx), baudrate(baudrate) {}

void RS485Server::start() {
  Serial1.begin(baudrate, SERIAL_8N1, rx, tx);
  pinMode(comm, OUTPUT);
  digitalWrite(comm, LOW);
}

void RS485Server::stop() {
  Serial1.end();
  pinMode(comm, INPUT);
  digitalWrite(comm, LOW);
  pinMode(rx, INPUT);
  digitalWrite(rx, LOW);
  pinMode(tx, INPUT);
  digitalWrite(tx, LOW);
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
  String command = Serial1.readStringUntil('\n');
  command.trim();
  return command;
}*/

String RS485Server::receiveFrom(int target, bool longTimeout) {
  String response = "";
  unsigned long startTime = millis();
  bool timeout = true;

  int timeoutDuration = longTimeout ? 30*TIMEOUT : TIMEOUT; // Adjust timeout duration based on longTimeout flag
  while (millis() - startTime < timeoutDuration) {
    if (Serial1.available()) {
      char c = Serial1.read();
      if (c == '\n' || c == '\r' || c == '\0') {
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
  if (recipient.equals(String(target)) || recipient.equals("*")) {
    response = getBody(response);
  } else {
    Serial.println("ERROR: Received message not for this target: " + recipient);
    response = "";  // Not for this target
  }
  response.trim();
  return response;  // Convert to uppercase for consistency
}

void RS485Server::transmitTo(int target, String message) {
  delay(200);
  digitalWrite(comm, HIGH);
  delayMicroseconds(100);

  Serial1.println((String(target) + " " + message).c_str());
  Serial1.flush();
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
