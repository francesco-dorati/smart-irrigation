#ifndef RS485CLIENT_H
#define RS485CLIENT_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class RS485Client {
 public:
  RS485Client(String id, int comm, int rx, int tx);
  void begin(long baudrate);
  bool available();
  String receiveCommand();
  void transmit(String message);

 private:
  String id;
  int comm;
  SoftwareSerial serial;
  unsigned long TIMEOUT = 100;  // Timeout for receiving messages

  String getRecipient(String command);
  String getBody(String command);
};

#endif  // RS485CLIENT_H