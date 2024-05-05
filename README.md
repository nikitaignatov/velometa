# velometa

Bike computer for training purpose. The goal is build a device that can present the required information during trainng rides outdoors or indoors.


## Core Sensors

Cores sensors are mainly commercial BLE devices, that provide metrics like HR, Watt, Speed, Cadence and so on. Typically these sensor will provide a measurement once every second.

- [x] Watt
- [x] HR
- [x] Speed
- Cadence

### Sensors
  

## Experimental Sensors

- [x] GPS
- [x] Static Pressure/Altitude
- [x] Differential Pressure/Pitot tube
- [x] Humidity
- [x] Temperature
- Slope
- Road Vibration
- [x] Airspeed
- [x] Wind Speed
- [x] Position
  - [x] Simple distance tracking (VL53L0X)
  - Advanced multi area distance tracking (VL53L7CX ultrawide FoV)
- Wind Angle
- Tire Pressure
- O2
- CO2

### Sensors 

Sensors will be connected via I2C bus. 
---

#### BME280
[Webpage](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/) 
/ 
[Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf)

Default address: **0x77**

Used to measure the following parameters

| Parameter         | output unit | min     | max      |
| ----------------- | ----------- | ------- | -------- |
| air pressure      | Pa          | 300 hPa | 1100 hPa |
| air temperature   | °C          | -40 °C  | 85 °C    |
| relative humidity | %           | 0 %     | 100 %    |
---
#### BMP581


---
#### XGZP6897D

[Webpage](https://cfsensor.com/product/i2c-differential-pressure-sensor-xgzp6897d/) 
/ 
[Datasheet](https://cfsensor.com/wp-content/uploads/2022/11/XGZP6897D-Pressure-Sensor-V2.7.pdf)

Default address: **0x6D**

Used to measure the following parameters

| Parameter             | output unit | min      | max     |
| --------------------- | ----------- | -------- | ------- |
| differential pressure | Pa          | -1000 Pa | 1000 Pa |
| temperature           | °C          | -30 °C   | 120 °C  |

The sensor has built in temperature compensation in the following temperature range: 0..60 °C.


---
#### MPU9250
#### MPU6050
#### BMI160
#### BMI270
#### BMI088
#### ADXL345
#### ADXL355
---
#### VL53L0X
[Webpage](https://www.st.com/en/imaging-and-photonics-solutions/vl53l0x.html) 
/ 
[Datasheet](https://www.st.com/resource/en/datasheet/vl53l0x.pdf)

Default address: **0x52** or **0x29** 

Used to measure the following parameters

| Parameter | output unit | min  | max     |
| --------- | ----------- | ---- | ------- |
| distance  | mm          | 0 mm | 2000 mm |

## Calculated Metric

- [x] Zones
  - [x] HR
  - [x] Power
- ride ftp
- Avg Weighted Power
- Training Load
- W'
- W'bal
- Work above threshold
- HR drift (decoupling)
- CdA

## Sensor connectivity / integration

- BLE for the commercial sensors.

## Data Storage

- Store Timeseries of all connected sensors at all times when device is turned on.
- Activity start/end/pause are stored as markers, so no data is lossed if forgot to start.

## Random Ideas

- Support multi-sensor for 1 metric (2 power meters)
- Control the training bike (adjust resistance ERG)


## Architecture

```mermaid

flowchart TD

subgraph "Peripheral Devices"
  epaper("Epaper Display 4.2inch")
  35screen("WT32-SC01-PLUS")
  7screen("ZX7D00CE01S")
  gps_module("GPS Module")
end

subgraph "Bluetooth LE Sensors"
  bike("Bike Sensors")
  trainer("Tainer Sensors")
  hr_monitor("HR Monitor")
end

subgraph "Microcontroller"
  hspi("HSPI")
  spi("SPI")
  spi---epaper
  hspi---35screen
  hspi---7screen
  ble("BLE")
  ble---bike
  ble---trainer
  ble---hr_monitor  
  uart---gps_module
  uart("UART")
end

subgraph "Core1"
  lvgl("LVGL")
  gfx("GxEPD2")
end

subgraph "Core0"
  sdcard("SDCard")
  gps("GPS")
  speed("Speed")
  power("Power")
  hr("HR")
end

subgraph "Drivers"
  Core1("Core1")
  Core0("Core0")
  hr---ble
  power---ble
  speed---ble
  gps---uart
  lvgl---hspi
  gfx---spi
  sdcard---spi
end

subgraph "Middleware"
  memory("Memory")
  render("Render / UI Components")
  bitmap("Bitmap")
  memory---sdcard
  render---lvgl
  render---gfx
  bitmap---sdcard
end

subgraph "Computation"
  telemetry---memory
  telemetry("Metrics")
  telemetry---gps
  telemetry---speed
  telemetry---hr
  telemetry---power
end

subgraph "Application"
  Computation("Computation")
  screen("Screen")
  map_page("Map")
  page("page")
  screen---render
  screen---map_page
  map_page---bitmap
  map_page---telemetry
  page---telemetry
  screen---page
end

```


## Sensor Data Collection

```mermaid

flowchart LR

subgraph "Peripheral"
  power_sensor1("Assioma")
  power_sensor2("Wahoo Kicker")
  speed_sensor1("Speed Sensor")
  hr_monitor("HR Monitor")
  gps_module("GPS Module")
end

subgraph "Driver"
  ble("BLE")
  gps("GPS")
  hr_monitor---ble  
  speed_sensor1---ble
  power_sensor1---ble
  power_sensor2---ble
  gps_module---gps
end

subgraph "Sensor"
  speed("Speed")
  power("Power")
  hr("HR")
  time("Time")
  elevation("Elevation")
  location("Location")
  ble---hr
  ble---power
  ble---speed
  gps---speed
  gps---elevation
  gps---time
  gps---location
end

subgraph "Computation"
  f("Filter")
  m("Metrics")
  telemetry("Raw Measurements")
  telemetry---f
  f---m
  time---telemetry
  location---telemetry
  elevation---telemetry
  speed---telemetry
  hr---telemetry
  power---telemetry
end

```
