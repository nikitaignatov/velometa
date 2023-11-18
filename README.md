# VeloHUB

Bike computer for training purpose. The goal is build a device that can present the required information during trainng rides outdoors or indoors.

## Sensor reading

- Watt
- HR
- Speed
- Cadance
- GPS

## Experimental

- Static Pressure/Altitude
- Differential Pressure/Pitot tube
- Humidity
- Temperature
- Slope
- Road Vibration
- Airspeed
- Wind Speed
- Wind Angle
- Tire Pressure

## Calculated Metric

- normalized Power
- training stress value
- W'
- Work above threshold
- HR drift (decoupling)
- CdA
- Zones
  - HR
  - Power
- ride ftp

## Sensor connectivity(integration

- BLE for the commercial sensors.

## Data Storage

- Store Timeseries of all connected sensors at all times when device is turned on.
- Activity start/end/pause are stored as markers, so no data is lossed if forgot to start.

## Random Ideas

- Support multi-sensor for 1 metric (2 power meters)
- Control the training bike (adjust resistance)

## Architecture

```mermaid

flowchart TD

subgraph "Peripheral Devices"
  epaper("Epaper Display 4.2inch")
  power_sensor1("Assioma")
  power_sensor2("Wahoo Kicker")
  speed_sensor1("Speed Sensor")
  hr_monitor("HR Monitor")
  gps_module("GPS Module")
end
subgraph "Microcontroller"
  Core1("Core1")
  Core0("Core0")
end

subgraph "Core1"
  spi("SPI")
  uart("UART")
  uart---gps_module
  spi---epaper
end

subgraph "Core0"
  ble("BLE")
  ble---speed_sensor1
  ble---power_sensor1
  ble---power_sensor2
  ble---hr_monitor  
end

subgraph "Drivers"
  sdcard("SDCard")
  gfx("GxEPD2")
  gps("GPS")
  speed("Speed")
  power("Power")
  hr("HR")
  hr---ble
  power---ble
  speed---ble
  gps---uart
  gfx---spi
  sdcard---spi
end

subgraph "Middleware"
  memory("Memory")
  render("Render")
  bitmap("Bitmap")
  memory---sdcard
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
