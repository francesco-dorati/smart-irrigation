# Final Demo

## Main Loop:
1. `LOAD DATA`: if memory is empty load data from SD card
2. `TURN ON PLANTS`: turn the main realy on
3. for each plant:
    1. `STATUS REQUEST`: request plant status
    2. `WATERING DECISION`: decide if and how much water the plant
    3. `LOG`: log activities in `history.csv`
4. `WAKEUP DECISION`: decide when to wake up next
5. `DEEP SLEEP`: deep sleep until RTC wakeup call 

## Memory Files:
- `plants.csv`
    - `id`: id of the plant
    - `name`: name of the plant
    - `type`: type of plant
    - `saucerCapacity`: capacity of the saucer    
- `history.csv`
    - `time`: date time stamp
    - `id`: plant id or `-` for Main Unit
    - `action`: between `STATUS`, `WATER`, `WAKEUP`, `SLEEP`, `RELAY_ON`, `RELAY_OFF`
    - `description`: description of the action (status, uptime, etc)

## Watering Process:

### Data for each Plant
 - `humidity`: current plant humidity (percentage)
 - `saucerEmpty`: wether saucer is empty
 - `saucerCapacity`: water capacity (or time to fill) of the plant saucer [ml or seconds]

### Type of Plants
  `SUCCULENT`, `FERN`, `HOUSEPLANT`, `MEDITERRANEAN`, `VEGETABLE`  
  Each of these plant types determines the values for these variables:
  - `minHumidity`: minimum soil humidity -> starts watering when reached
  - `optimalHumidity`: optimal soil humidity -> stops watering when reached
### Plant States
- `WATERING`: plant is being gradually watered (until humidity reaches `optimalHumidity`)
- `ABSORBING`: plant is absorbing water (waiting for humidity to reach `minHumidity`)

### Other External Data
- `temperature`: outside temperature
- `airHumidity`: outside air humidity

  
### Watering Process
The watering process proceeds gradually by filling the saucer by a specific amount and waiting for the plant to absorb it.
1. `SAUCER CHECK`: Check if the saucer is empty to receive water
  - if `ABSORBING` state:
    1. `CHECK`: Plant needs to be watered if `humidity <= minHumidity`
    2. `CALCULATE`: The amount of water to add is `saucerCapacity * (optimalHumidity - humidity)*2`
  - if `WATERING` state:
    1. `CHECK`: Plant needs to stop the watering process if `humidity >= optimalHumidity * 0.9`
    2. `CALCULATE`: The amount of water to add is `saucerCapacity * (optimalHumidity - humidity)*2`
       
## Interfaces
- CLI -> Main Unit 
  - `INFO`: local info (without switching on plants)
  - `STATUS`: info about memory, and for each plant list current humidity and last watered time
  - `PLANT LIST`: list plants
  - `PLANT NEW`: new plant
  - `PLANT REMOVE`: remove plant
  - `PLANT INFO <id>`: prints local plant info
  - `PLANT STATUS <id>`: pings plant
  - `PLANT WATER <id>`: checks if plant needs water and if so waters it
    
- Main Unit -> Plant Unit
  - Status reading:  
      `STATUS` -> `<ID> <humidity> <saucerEmpty>`
  - Watering command:  
    `WATER <ml>` -> `<ID> DONE`
  - Humidity calibration  
    `HUMIDITY_CALIBRATION` -> `<ID> <sensor_voltage> <sensor_value>` 
  - Saucer capacity calibration:  
      `CAPACITY_CALIBRATION` -> `<ID> [OK/KO]`
    - Start calibration: `START`
    - Stop calibration: `STOP` -> `<ID> <ml>`        

## Preditions:
- predict water consumption by plant, given plant info and temperature
- predict saucer water absobtion given outside temperature, humidity

