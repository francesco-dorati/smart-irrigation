#include <Arduino.h>
#include <SoftwareSerial.h>

const int COMM_PIN = 2;
const int RX_PIN = 10; // RX pin for RS485
const int TX_PIN = 11; // TX pin for RS485

class RS485Server
{
public:
    RS485Server(int comm, int rx, int tx) : comm(comm), serial(rx, tx) {}

    void begin(long baudrate)
    {
        serial.begin(baudrate);
        pinMode(comm, OUTPUT);
        digitalWrite(comm, LOW);
    }

    bool available()
    {
        return serial.available();
    }

    String receiveResponse()
    {
        String command = serial.readStringUntil('\n');
        command.trim();
        return command;
    }

    void transmitTo(String target, String message)
    {
        digitalWrite(comm, HIGH);
        delayMicroseconds(10);
        serial.println((target + " " + message).c_str());
        serial.flush();
        digitalWrite(comm, LOW);
        Serial.println("sent: " + message);
    }

private:
    int comm;
    SoftwareSerial serial;
};

RS485Server rs485(COMM_PIN, RX_PIN, TX_PIN);

void setup()
{
    Serial.begin(9600);
    rs485.begin(9600);
    Serial.println("START\n");
}

void loop()
{
    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');
        rs485.transmitTo("*", command);
    }

    if (rs485.available())
    {
        String response = rs485.receiveResponse();
        Serial.println("received: " + getBody(response) + "\n");
    }
}

String getRecipient(String command)
{
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1)
        return "";
    return command.substring(0, spaceIndex);
}

String getBody(String command)
{
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex == -1)
        return "";
    return command.substring(spaceIndex + 1);
}