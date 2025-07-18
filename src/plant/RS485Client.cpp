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
  String command = serial.readStringUntil('\n');

  command.trim();
  if (command.equals("")) return "";

  String recipient = getRecipient(command);
  return (recipient.equals(id) || recipient.equals("*")) ? getBody(command)
                                                         : "";
}

void RS485Client::transmit(String message) {
  digitalWrite(comm, HIGH);
  delayMicroseconds(10);
  serial.println((id + " " + message).c_str());
  serial.flush();
  digitalWrite(comm, LOW);
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
