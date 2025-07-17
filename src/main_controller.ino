#include <Arduino.h>

const int COMM_PIN = 2;



void setup (){
    pinMode(COMM_PIN, OUTPUT);
    digitalwrite(COMM_PIN, LOW);
    Serial.begin(9600);
    Serial1.begin(9600)

}

void loop(){
    if (Serial.avaiable()){
        char c = Serial.read();
        switch (c) {
            case 'a': 
                transmit("H");
                break;
            case 'b':
                int sec = Serial.parseInt();
                transmit("P" + String(sec));
            }
        }
    if (Serial1.available()) {
    String response = Serial1.readStringUntil('\n'); // oppure un timeout con readString()
    Serial.print("Risposta da Serial1: ");
    Serial.println(response)
        }
    delay(50)
}

void transmit(const char * message){
    digitalWrite(COMM_PIN, HIGH); 
    delayMicroseconds(10);     

    Serial1.write(message);
    Serial1.flush();             
    digitalWrite(COMM_PIN, LOW);
}