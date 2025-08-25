#include "SDCard.h"

SDCard::SDCard(gpio_num_t CS_PIN, gpio_num_t SCK_PIN, gpio_num_t MISO_PIN, gpio_num_t MOSI_PIN)
    : csPin(CS_PIN), sckPin(SCK_PIN), misoPin(MISO_PIN), mosiPin(MOSI_PIN) {}

bool SDCard::start() {
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    SPI.begin(sckPin, misoPin, mosiPin, csPin);
    return SD.begin(csPin);
}

void SDCard::stop() {
    SD.end();
    SPI.end();  // Explicitly end SPI to release bus fully

    // Set pins to high impedance (INPUT) for power-off
    pinMode(csPin, INPUT);
    pinMode(sckPin, INPUT);
    pinMode(misoPin, INPUT);
    pinMode(mosiPin, INPUT);
}

int SDCard::getTotalSpace() {
    return SD.cardSize() / (1024 * 1024 * 1024); // Return size in GB
}

int SDCard::getFreeSpace() {
    uint64_t total = SD.totalBytes();
    uint64_t used  = SD.usedBytes();
    return (total - used) / (1024 * 1024 * 1024); // Free space in GB
}

bool SDCard::testWrite() {
    File file = SD.open(SD_CARD_TEST_FILE, FILE_WRITE);
    if (!file) return false;

    file.println("This is a test write.");
    file.close();
    return true;
}

String SDCard::testRead() {
    File file = SD.open(SD_CARD_TEST_FILE);
    if (!file) return "ERROR OPENING FILE";

    String content = "";
    while (file.available()) {
        content += (char)file.read();
    }
    file.close();
    return content;
}

void SDCard::addPlant(Plant *plant) {
    File file = SD.open(SD_CARD_PLANTS_FILE, FILE_APPEND);
    if (!file) {
        Serial.println("ERROR: Unable to open plants file for appending");
        return false;
    }
    file.println(String(plant->getId()) + "," + plant->getName() + "," +
                 String(plant->getState()) + "," +
                 plant->getWaterPreference().toString() + "," +
                 String(plant->saucerCapacity));
    file.close();
    return true;
}

bool SDCard::loadPlants(Plant *plants[], int maxPlants, RS485Server& rs485) {
    File file = SD.open(SD_CARD_PLANTS_FILE);
    if (!file) {
        Serial.println("ERROR: Unable to open plants file");
        return false;
    }

    int count = 0;
    while (file.available() && count < maxPlants) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.length() == 0) continue; // Skip empty lines

        int comma1 = line.indexOf(',');
        int comma2 = line.indexOf(',', comma1 + 1);
        int comma3 = line.indexOf(',', comma2 + 1);
        int comma4 = line.indexOf(',', comma3 + 1);
        int comma5 = line.indexOf(',', comma4 + 1);
        int comma6 = line.indexOf(',', comma5 + 1);

        // Parse the line into plant properties
        int id = line.substring(0, comma1).toInt();
        String name = line.substring(comma1 + 1, comma2);
        PlantState state = static_cast<PlantState>(line.substring(comma2 + 1, comma3).toInt());
        String waterPreferenceName = line.substring(comma3 + 1, comma4);
        float waterPreferenceMinHumidity = line.substring(comma4 + 1, comma5).toFloat();
        float waterPreferenceOptimalHumidity = line.substring(comma5 + 1, comma6).toFloat();
        int saucerCapacity = line.substring(comma6 + 1).toInt();

        WaterPreference waterPreference = WaterPreference::fromValues(waterPreferenceName, waterPreferenceMinHumidity, waterPreferenceOptimalHumidity);

        // Create a new Plant object and add it to the array
        plants[count++] = new Plant(rs485, id, name, state, waterPreference, saucerCapacity);
    }
    file.close();
    return true;
}