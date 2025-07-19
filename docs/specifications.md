# FINAL DEMO

## when powered on:
```
load plant list from SD
turn plants on
for each plant:
  get_humidity(plant_id)
  if humidity < min_humidity(plant_id):
      water_plant(plant_id, humidity)
  save history to SD
```

## memory files:
- plants.csv [plant_id, name, watering_preferences]
- history.csv [time, action_performed, plant_id]

## access points
- USB (debug)
  - PLANT MANAGEMENT commands:
      - PLANT NEW <plant_id> <name>: new plant
      - PLANT LIST: list plants
      - PLANT REMOVE <plant_id> : remove plant
  - STATUS commands:
      - STATUS: info about memory, and for each plant list current humidity and last watered time
      - INFO: local info (without switching on plants)
  - ACTIONS commands:
      - WATER <plant_id> <milliliters>: water a plant
      - CHECK <plant_id>: checks if plant needs water and if so water it 
          


