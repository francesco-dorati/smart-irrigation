#ifndef RS485SERVER_H
#define RS485SERVER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class RS485Server {
 public:
  RS485Server(int comm, int rx, int tx);
  void begin(long baudrate);
  bool available();
  String receiveResponse();
  void transmitTo(String target, String message);

 private:
  int comm;
  SoftwareSerial serial;
};

#endif  // RS485SERVER_H