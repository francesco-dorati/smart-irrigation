#include "RS485Server.h"

RS485Server::RS485Server(int comm, int rx, int tx)
    : comm(comm), serial(rx, tx) {}

void RS485Server::begin(long baudrate) {
  serial.begin(baudrate);
  pinMode(comm, OUTPUT);
  digitalWrite(comm, LOW);
}

bool RS485Server::available() { return serial.available(); }

String RS485Server::receiveResponse() {
  String command = serial.readStringUntil('\n');
  command.trim();
  return command;
}

void RS485Server::transmitTo(String target, String message) {
  digitalWrite(comm, HIGH);
  delayMicroseconds(10);
  serial.println((target + " " + message).c_str());
  serial.flush();
  digitalWrite(comm, LOW);
  Serial.println("sent: " + message);
}
