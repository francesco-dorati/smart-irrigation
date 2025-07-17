int sensorPin = A0;
int pumpPin = 4;

int value = 0;
float voltage = 0;
float percentage = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH);
}

void loop() {
  value = analogRead(sensorPin);
  voltage = analogToVoltage(value);
  percentage = voltageToPercentage(voltage);
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print(" V, percentage: ");
  Serial.print(percentage);
  Serial.println(" %");

  if (percentage > 50) {
    digitalWrite(pumpPin, LOW);
    Serial.println("PUMP ON");
  } else {
    digitalWrite(pumpPin, HIGH);
    Serial.println("PUMP OFF");
  }

  delay(500); 
}

const float MAX_VOLTAGE = 5;
float analogToVoltage(int analogValue) {
  return (value * MAX_VOLTAGE)/1023.0;
}

const float DRY_VOLTAGE = 3.03;
const float WET_VOLTAGE = 1.08;
float voltageToPercentage(float voltage) {
  return 100.0 * (voltage - DRY_VOLTAGE) / (WET_VOLTAGE - DRY_VOLTAGE);
}
