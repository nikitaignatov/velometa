#include "esp_log.h"

#define DEVELOPMENT
#define SETUP 2

#if SETUP == 1
#define DEVICE_NAME_HR "HR6 0031452"
#define DEVICE_NAME_HR2 "HR8 30743"
#define DEVICE_NAME_POWER "KICKR CORE 28DF" // f9:4a:e9:9a:67:4d
#define DEVICE_NAME_SPEED "XXX"
#define DEVICE_NAME_AIRSPEED "VELOMETA_AIRSPEED"
#define DEVICE_NAME_GPS "XXX"
#endif
#if SETUP == 2
#define DEVICE_NAME_HR "HR6 0031452"
#define DEVICE_NAME_HR2 "HR8 30743"
#define DEVICE_NAME_POWER "ASSIOMA13805L"
#define DEVICE_NAME_SPEED "CS8L-S 45539"
#define DEVICE_NAME_AIRSPEED "VELOMETA_AIRSPEED"
#define DEVICE_NAME_GPS "XXX"
#endif

#define FEATURE_SCREEN_ENABLED
#define FEATURE_GPS_ENABLED
#define FEATURE_HR_ENABLED
#define FEATURE_POWER_ENABLED
// #define FEATURE_SPEED_ENABLED

#define BOARD_LILY_WRIST 1
#define BOARD_LILY_154 2
#define BOARD_WAVESHARE 3
#define WTSC01_PLUS 4
#define BOARD WTSC01_PLUS

#define MSG_NEW_SPEED 1
#define MSG_NEW_POWER 2
#define MSG_NEW_HR 3
#define MSG_NEW_HEADING 4
#define MSG_NEW_GPS 5
#define MSG_NEW_HR_W 6

#if BOARD == WTSC01_PLUS
#define VM_I2C_0_SDA 11
#define VM_I2C_0_SCL 10

#define VM_I2C_1_SDA 13
#define VM_I2C_1_SCL 12

#define VM_I2C_2_SDA 21
#define VM_I2C_2_SCL 14
#endif

#define POSITION_SENSOR_ADDRESS 0x29
#define POSITION_SENSOR_SDA VM_I2C_0_SDA
#define POSITION_SENSOR_SCL VM_I2C_0_SCL

#define ENVIRONMENTAL_SENSOR_ADDRESS 0x77
#define ENVIRONMENTAL_SENSOR_SDA VM_I2C_0_SDA
#define ENVIRONMENTAL_SENSOR_SCL VM_I2C_0_SCL

#define AIR_PRESSURE_SENSOR_ADDRESS 0x6D
#define AIR_PRESSURE_SENSOR_SDA VM_I2C_0_SDA
#define AIR_PRESSURE_SENSOR_SCL VM_I2C_0_SCL
