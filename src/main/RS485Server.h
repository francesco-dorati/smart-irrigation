#ifndef RS485SERVER_H
#define RS485SERVER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class RS485Server {
 public:
  RS485Server(int comm, int rx, int tx);
  void begin(long baudrate);
  bool available();
  //String receiveResponse();
  String receiveFrom(int target, bool avoidTimeout = false);
  void transmitTo(int target, String message);

 private:
 const unsigned long TIMEOUT = 1000;  // Timeout for receiving messages
 int comm;
 SoftwareSerial serial;

  String getRecipient(String command);
  String getBody(String command);
  
};

#endif  // RS485SERVER_H