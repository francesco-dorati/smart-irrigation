# Final Demo

## Main Loop:
1. **LOAD DATA**  
   - If `plants.csv`, `history.csv` or `weather.csv` is not in RAM, load them from SD card.  

     - **weather.csv** fields:  
       - `date` (YYYY-MM-DD)  
       - `T_max` (°C), `T_min` (°C)  
       - `Rn` (MJ/m²·day), `RH` (%)  
       - `u2` (m/s)

2. **TURN ON PLANTS**  
   - Activate the main relay (`RELAY_ON`).

3. **FOR EACH PLANT**  

   1. **STATUS REQUEST**  
      - Send `STATUS` → receive `<ID> <humidity> <saucerEmpty>`.

   2. **WATERING DECISION**  
      - **If** `saucerEmpty == true` (state `ABSORBING`): 

        1. **Compute reference evapotranspiration (ET₀)** using FAO Penman–Monteith:
           ```
           ET_{0} =
           (0.408 * Δ * (Rn - G) + (900/(T + 273)) * γ * u2 * (es - ea))
           / (Δ + γ * (1 + 0.34 * u2))
           ```
           - T = (T_max + T_min)/2  
           - G ≈ 0 (daily balance)  
           - Δ, γ, es, ea computed from T and RH.  

        2. **Select crop coefficient Kc** based on `type` and growth stage (use mid‑season Kc_mid by default):

           type            | Photosynthesis | Kc_mid
           ----------------|----------------|-------
           SUCCULENT       | CAM            | 0.30
           MEDITERRANEAN   | C3 (dry‑adapt) | 0.60
           HOUSEPLANT      | C3 (indoor)    | 0.80
           FERN            | C3 (humid)     | 1.00
           VEGETABLE       | C3/C4          | 1.10

        3. **Compute crop evapotranspiration**:
           ```
           ETc = Kc * ET0   [mm/day]
           ```
        4. **Convert ETc to water volume (ml)**:  
           - If saucer area A (m²) is known:
             ```
             water_ml = ETc [mm] * A [m2] * 1000
             ```
           - Otherwise scale by `saucerCapacity`:
             ```
             water_ml = saucerCapacity * (ETc / ET_ref)
             ```
             with ET_ref = 5 mm/day (approx. one full saucer).

        5. **Decision**: if `humidity <= minHumidity`, send  
           `WATER <water_ml>`  
           and set state = `WATERING`.

      - **Else if** state == `WATERING`:  
        1. **Check stop condition**: if  
           `humidity >= optimalHumidity * 0.9`  
           then send `WATER 0` (or `RELAY_OFF`) to stop.  
        2. **(Optional)**: recalculate ETc and adjust `water_ml` if continuing.

   3. **LOG**  
      - Append to `history.csv`: timestamp, `id`, `action`, `description`.

4. **WAKEUP DECISION**  
   - Based on predicted ET₀ for coming days (hot/dry → more frequent cycles), schedule next RTC wakeup.

5. **DEEP SLEEP**  
   - Enter deep sleep until RTC.

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
- weather.csv
    - `date`,
    - `T_max`,
    - `T_min`,
    - `Rn`,
    - `RH`,
    - `u2`.

## Watering Process:

### Per-plant Data
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
- `temperature` (T_max, T_min)
- `airHumidity` (RH)
- `Rn`, `u2`



/* 
### Watering Process
The watering process proceeds gradually by filling the saucer by a specific amount and waiting for the plant to absorb it.
1. `SAUCER CHECK`: Check if the saucer is empty to receive water
  - if `ABSORBING` state:
    1. `CHECK`: Plant needs to be watered if `humidity <= minHumidity`
    2. `CALCULATE`: The amount of water to add is `saucerCapacity * (optimalHumidity - humidity)*2`
  - if `WATERING` state:
    1. `CHECK`: Plant needs to stop the watering process if `humidity >= optimalHumidity * 0.9`
    2. `CALCULATE`: The amount of water to add is `saucerCapacity * (optimalHumidity - humidity)*2`
*/


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

## Future Predictions & AI
- Water consumption: use daily ETc to predict volumes.
- Saucer absorption: model rate based on T and RH.
- AI extensions: time-series ML or reinforcement learning for
optimization.

