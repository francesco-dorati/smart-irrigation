# FINAL DEMO

## Main Loop:
```
load data from SD
turn on plants
for each plant:
  get_humidity(plant_id)
  if humidity < min_humidity(plant_id):
      water_plant(plant_id, humidity)
  save history to SD
```
when plant is below minimum -> water until optimal humidity
water and set time where to check again

## memory files:
- plants.csv [plant_id, name, watering_preferences]
- history.csv [time, action_performed, plant_id]

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
  - `STATUS`: info about memory, and for each plant list current humidity and last watered time
  - `INFO`: local info (without switching on plants)
  - `PLANT LIST`: list plants
  - `PLANT NEW <plant_id> <name>`: new plant
  - `PLANT REMOVE <plant_id>`: remove plant
  - `PLANT PING <plant_id>`: pings plant
  - `PLANT CHECK <plant_id>`: checks plant humidity and waters it if necessary
  - `PLANT INFO <plant_id>`: prints local plant info
- Main Unit -> Plant Unit
  - Status reading:  
      `STATUS` -> `<ID> <humidity> <saucerEmpty>`
  - Watering command:  
    `WATER <ml>` -> `<ID> DONE`
  - Saucer capacity calibration:  
      `CAPACITY_CALIBRATION` -> `<ID> [OK/KO]`
    - Start calibration: `START`
    - Stop calibration: `STOP` -> `<ID> <ml>`

          

## Preditions:
- predict water consumption by plant, given plant info and temperature
- predict saucer water absobtion given outside temperature, humidity

