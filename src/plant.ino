#include <Arduino.h>
#include <SoftwareSerial.h>

const String PLANT_ID = "0";  // Unique ID for this plant

const int SENSOR_PIN = A0;
const int LED_PIN = 2;
const int COMM_PIN = 3;  // LOW: listening, HIGH: sending
const int PUMP_PIN = 4;  // LOW: ON, HIGH: OFF
const int RX_PIN = 10;   // RX pin for RS485
const int TX_PIN = 11;   // TX pin for RS485

class LED {
 public:
  LED(int p) : pin(p) {}

  void begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  void setOn() { digitalWrite(pin, HIGH); }

  void setOff() { digitalWrite(pin, LOW); }

  void blink(int seconds) {
    setOn();
    delay(seconds * 1000);
    setOff();
  }

 private:
  int pin;
};

class HumiditySensor {
 public:
  HumiditySensor(int p) : pin(p) {}

  void begin() { pinMode(pin, INPUT); }

  int getHumidity() {
    return 100.0 * (getVoltage() - DRY_VOLTAGE) / (WET_VOLTAGE - DRY_VOLTAGE);
  }

  float getVoltage() { return (getValue() * MAX_VOLTAGE) / 1023.0; }

  int getValue() { return analogRead(pin); }

 private:
  int pin;
  const float MAX_VOLTAGE = 5.0;
  const float DRY_VOLTAGE = 3.27;  // 3.04 on local
  const float WET_VOLTAGE = 1.18;  // 1.11 on local
};

class Pump {
 public:
  Pump(int p) : pin(p), on(false) {}

  void begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);  // Start with pump OFF
  }

  void setOn() {
    digitalWrite(pin, LOW);  // Turn pump ON
    on = true;
  }

  void setOff() {
    digitalWrite(pin, HIGH);  // Turn pump OFF
    on = false;
  }

  void isOn() { return on; }

  void activate(int seconds) {
    setOn();
    delay(seconds * 1000);
    setOff();
  }

 private:
  int pin;
  bool on;
};

class RS485Client {
 public:
  RS485Client(String id, int comm, int rx, int tx)
      : id(id), comm(comm), serial(rx, tx) {}

  void begin(long baudrate) {
    serial.begin(baudrate);
    pinMode(comm, OUTPUT);
    digitalWrite(comm, LOW);
  }

  bool available() { return serial.available(); }

  String receiveCommand() {  // Ensure command is from this plant
    String command = serial.readStringUntil('\n');

    command.trim();
    if (command.equals("")) return "";

    String recipient = getRecipient(command);
    return (recipient.equals(id) || recipient.equals("*")) ? getBody(command)
                                                           : "";
  }

  void transmit(String message) {
    digitalWrite(comm, HIGH);
    delayMicroseconds(10);
    serial.println((id + " " + message).c_str());
    serial.flush();
    digitalWrite(comm, LOW);
    Serial.println("sent: " + message);
  }

 private:
  String id;
  int comm;
  SoftwareSerial serial;

  String getRecipient(String command) {
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1) return "";
    return command.substring(0, spaceIndex);
  }

  String getBody(String command) {
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1) return "";
    return command.substring(spaceIndex + 1);
  }
};

RS485Client rs485(PLANT_ID, COMM_PIN, RX_PIN, TX_PIN);
LED led(LED_PIN);
HumiditySensor humiditySensor(SENSOR_PIN);
Pump pump(PUMP_PIN);

void setup() {
  Serial.begin(9600);

  rs485.begin(9600);
  led.begin();
  humiditySensor.begin();
  pump.begin();

  led.blink(1);
  Serial.println("START");
}

void loop() {
  // RS485 communication
  if (rs485.available()) {
    String command =
        rs485.receiveCommand();  // Ensure command is for this plant
    if (!command.equals("")) {
      interpretCommand(command);
    }
  }

  // Serial communication (DEBUG)
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    Serial.println("debugging: \"" + command + "\"");
    interpretDebug(command);
  }
}

void interpretCommand(String command) {
  if (command.equals("PING")) {
    rs485.transmit("PONG");
    led.blink(1);
  } else if (command.equals("HUMIDITY")) {
    int humidity = humiditySensor.getHumidity();
    rs485.transmit(String(humidity) + " " +
                   String(humiditySensor.getVoltage()));
  } else if (command.startsWith("WATER")) {
    int seconds = command.substring(6).toInt();
    if (seconds < 0) return;  // Invalid command
    pump.activate(seconds);
  } else {
    Serial.println("Unknown command: \"" + command + "\"");
  }
}

void interpretDebug(String command) {
  if (command.equals("PING")) {
    Serial.println("PONG");
    led.blink(1);
  } else if (command.equals("HUMIDITY")) {
    int value = humiditySensor.getValue();
    float voltage = humiditySensor.getVoltage();
    int humidity = humiditySensor.getHumidity();
    Serial.println("Humidity: " + String(humidity) + "%, Voltage: " +
                   String(voltage) + "V, value: " + String(value));
  } else if (command.startsWith("WATER")) {
    int seconds = command.substring(6).toInt();
    if (seconds < 0) return;  // Invalid command
    pump.activate(seconds);
  } else {
    Serial.println("Unknown command: " + command);
  }
}
