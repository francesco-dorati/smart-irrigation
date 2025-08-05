#ifndef RS485SERVER_H
#define RS485SERVER_H

#include <Arduino.h>

class RS485Server {
 public:
  RS485Server(int comm, int rx, int tx, long baudrate);
  void start();
  void stop();
  bool available();
  //String receiveResponse();
  String receiveFrom(int target, bool avoidTimeout = false);
  void transmitTo(int target, String message);

 private:
 const unsigned long TIMEOUT = 5000;  // Timeout for receiving messages
 int comm;
 int rx;
 int tx;
 long baudrate;

  String getRecipient(String command);
  String getBody(String command);
  
};

#endif  // RS485SERVER_H