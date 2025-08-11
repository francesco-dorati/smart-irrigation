#include "Controller.h"

Controller::Controller()
    : isPowerOn(false),
      tempSensor(DHT_PIN, DHT11),
      rs485(COMM_PIN, RX_PIN, TX_PIN, 9600) {}

void Controller::init() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);  // Set pin to LOW to avoid floating state
  pinMode(SHIFTER_POWER_PIN, OUTPUT);  // Set pin to LOW to avoid floating state
  pinMode(RTC_WAKEUP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_WAKEUP_PIN, INPUT);
  
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(POWER_PIN, LOW);  // Ensure pin is LOW initially
  digitalWrite(SHIFTER_POWER_PIN, LOW);  // Ensure pin is LOW initially

  // RTC begin
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    // Set time only the first time or after battery loss
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  /* remove */ plants[0] =
      new Plant(rs485, 0, "Test Plant", PlantState::ABSORBING,
                WaterPreference::MEDITERRANEAN, 100);
}

Plant* Controller::getPlant(int id) {
  turnOn();
  for (int i = 0; i < MAX_PLANTS; i++) {
    if (plants[i] != nullptr && plants[i]->getId() == id) {
      return plants[i];
    }
  }
  return nullptr;  // Plant not found
}

Plant** Controller::getPlants() {
  return plants;  // Return the array of plant pointers
}

bool Controller::isOn() { return isPowerOn; }

void Controller::turnOn() {
  pinMode(POWER_PIN, OUTPUT);  // Set pin to OUTPUT to control power
  digitalWrite(POWER_PIN, HIGH);

  pinMode(SHIFTER_POWER_PIN, OUTPUT);
  digitalWrite(SHIFTER_POWER_PIN, HIGH);

  // Allow some time for the power to turn on
  if (!isPowerOn) {
    // Powering on the system
    delay(1000);  // Allow some time for the power to stabilize
    rs485.start();
    tempSensor.begin();
    delay(1000);
  }
  isPowerOn = true;
}

void Controller::turnOff() {
  digitalWrite(SHIFTER_POWER_PIN, LOW);  // Turn off power
  delay(10);

  digitalWrite(POWER_PIN, LOW);  // Turn off power
  delay(10); 

  // state
  if (isPowerOn) {
    rs485.stop();
    pinMode(DHT_PIN, INPUT); 
    digitalWrite(DHT_PIN, LOW);  // Ensure pin is LOW
  }
  isPowerOn = false;
}

void Controller::sleep(int seconds) {
  turnOff();

  rtc_gpio_init((gpio_num_t)SHIFTER_POWER_PIN);
  rtc_gpio_set_direction((gpio_num_t)SHIFTER_POWER_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
  rtc_gpio_set_level((gpio_num_t)SHIFTER_POWER_PIN, 0);

  rtc_gpio_init((gpio_num_t)POWER_PIN);
  rtc_gpio_set_direction((gpio_num_t)POWER_PIN, RTC_GPIO_MODE_OUTPUT_ONLY);
  rtc_gpio_set_level((gpio_num_t)POWER_PIN, 0);

  // Clear any existing alarms
  rtc.disableAlarm(1);
  rtc.clearAlarm(1);

  DateTime now = rtc.now();
  rtc.setAlarm1(now + TimeSpan(0, 0, 0, seconds), DS3231_A1_Second);

  // Configure wakeup from external pin
  esp_sleep_enable_ext0_wakeup(RTC_WAKEUP_PIN, 0); // Wake when pin goes LOW
  esp_sleep_enable_ext1_wakeup((1ULL << BUTTON_WAKEUP_PIN), ESP_EXT1_WAKEUP_ANY_HIGH);


  Serial.println("Going to sleep for " + String(seconds) + " seconds...");
  delay(1000);
  esp_deep_sleep_start();
}


String Controller::getTimeString() {
    DateTime now = rtc.now();
    char buf[20]; // enough space for "yyyy-mm-dd hh:mm:ss"
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", 
      now.year(), 
      now.month(),
      now.day(), 
      now.hour(), 
      now.minute(), 
      now.second());
    return String(buf);
}

String Controller::getReadableTimeString() {
    DateTime now = rtc.now();
    const char* months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    char buf[30]; // enough space for "dd Month yyyy hh:mm"
    sprintf(buf, "%d %s %04d %02d:%02d",
            now.day(),
            months[now.month() - 1],
            now.year(),
            now.hour(),
            now.minute());
    return String(buf);
}

String Controller::getEnvironmentDataString() {
  turnOn();
  float humidity = tempSensor.readHumidity();
  float temperature = tempSensor.readTemperature();

  /*if (isnan(humidity) || isnan(temperature)) {
    // Retry once
    tempSensor.begin();
    delay(5000);
    humidity = tempSensor.readHumidity();
    temperature = tempSensor.readTemperature();
    Serial.println("Retrying DHT sensor read...");
  }*/

  if (isnan(humidity) || isnan(temperature)) {
    return "Failed to read from DHT sensor!";
  }

  return "Temperature: " + String((int)temperature) +
         "°C\nAir Humidity: " + String((int)humidity) + "%";
}

String Controller::getPlantStatusString(int id) {
  turnOn();
  String s = "";
  Plant* plant = getPlant(id);
  if (plant == nullptr) {
    s += "\n\t\tCurrently OFFLINE.";
    return s;
  }

  bool status = plant->loadStatus();

  String humidity = String((int)(plant->getHumidity() * 100));
  String saucerFull = plant->isSaucerFull() ? "FULL" : "EMPTY";
  int waterNeeded = plant->checkWaterNeeds(plant->getHumidity());

  s += "\t- [" + String(plant->getId()) + "] " + plant->getName();
  if (status) {
    s += "\n\t\tHumidity: \t" + humidity + "%";
    s += "\n\t\tSaucer: \t" + saucerFull;
    s += "\n\t\tNeeds Water: \t" +
         ((waterNeeded > 0) ? "YES, " + String(waterNeeded) + " %" : "NO");
  } else {
    s += "\n\t\tCurrently OFFLINE.";
  }

  return s;
}
