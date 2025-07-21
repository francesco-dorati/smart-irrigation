# FINAL DEMO

## when powered on:
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

## access points
- USB (debug)
  - STATUS commands:
      - STATUS: info about memory, and for each plant list current humidity and last watered time
      - INFO: local info (without switching on plants)
  - PLANT MANAGEMENT commands:
      - PLANT LIST: list plants
      - PLANT NEW <plant_id> <name>: new plant
      - PLANT REMOVE <plant_id>: remove plant
      - PLANT PING <plant_id>: pings plant
      - PLANT CHECK <plant_id>: checks plant humidity and waters it if necessary
      - PLANT INFO <plant_id>: prints local plant info

          

## preditions:
- predict in water consumption by plant, given plant info and temperature
- 
