#pragma once

#include "config.hpp"
#include "types.hpp"
#include <tuple>
#include <float.h>
#include "I2Cdev.h"
#include <math.h>
#include "Wire.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <optional>

class IMU
{
private:
    MPU6050 mpu;
    uint8_t _address;
    std::optional<float> _roll, _pitch, _yaw;
    bool ready = true;

    // MPU control/status vars
    bool dmpReady = false;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer


public:
    bool read_sensor();

    // orientation/motion vars
    Quaternion q;        // [w, x, y, z]         quaternion container
    VectorInt16 aa;      // [x, y, z]            accel sensor measurements
    VectorInt16 gg;      // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
    VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity; // [x, y, z]            gravity vector
    float euler[3];      // [psi, theta, phi]    Euler angle container
    float ypr[3];        // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

    std::optional<float> get_roll();
    std::optional<float> get_pitch();
    std::optional<float> get_yaw();
    std::optional<float> ax, ay, az, gx, gy, gz, qw, qx, qy, qz;

    void init();
};
