#include "imu.hpp"
#define OUTPUT_READABLE_YAWPITCHROLL

static const char *TAG = "environmental_sensor";
bool IMU::read_sensor()
{
    if (dmpReady)
    {
        // Wire.end();
        // Wire.begin(IMU_SENSOR_SDA, IMU_SENSOR_SCL, 400000);
        if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        { // Get the Latest packet
#ifdef OUTPUT_READABLE_QUATERNION
          // display quaternion values in easy matrix form: w x y z
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            Serial.print("quat\t");
            Serial.print(q.w);
            Serial.print("\t");
            Serial.print(q.x);
            Serial.print("\t");
            Serial.print(q.y);
            Serial.print("\t");
            Serial.println(q.z);
#endif

#ifdef OUTPUT_READABLE_EULER
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetEuler(euler, &q);
            Serial.print("euler\t");
            Serial.print(euler[0] * 180 / M_PI);
            Serial.print("\t");
            Serial.print(euler[1] * 180 / M_PI);
            Serial.print("\t");
            Serial.println(euler[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            _yaw = ypr[0] * 180 / M_PI;
            _pitch = ypr[1] * 180 / M_PI;
            _roll = ypr[2] * 180 / M_PI;
            mpu.dmpGetEuler(euler, &q);

            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGyro(&gg, fifoBuffer);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpConvertToWorldFrame(&aaWorld, &aaReal, &q);

            // ex = euler[0] * 180 / M_PI;
            // ay = euler[1] * 180 / M_PI;
            // az = euler[2] * 180 / M_PI;

            ax = aaWorld.x;
            ay = aaWorld.y;
            az = aaWorld.z;

            gx = aaReal.x;
            gy = aaReal.y;
            gz = aaReal.z;

            qw = q.w;
            qx = q.x;
            qy = q.y;
            qz = q.z;

            // Serial.print("ypr\t");
            // Serial.print(_yaw.value());
            // Serial.print("\t");
            // Serial.print(_pitch.value());
            // Serial.print("\t");
            // Serial.println(_roll.value());
#endif

#ifdef OUTPUT_READABLE_REALACCEL
            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.println(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
            // display initial world-frame acceleration, adjusted to remove gravity
            // and rotated based on known orientation from quaternion
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            Serial.print("aworld\t");
            Serial.print(aaWorld.x);
            Serial.print("\t");
            Serial.print(aaWorld.y);
            Serial.print("\t");
            Serial.println(aaWorld.z);
#endif
            return true;
        }
    }
    else
    {
        return false;
    }
}

std::optional<float> IMU::get_roll()
{
    return _roll;
}

std::optional<float> IMU::get_pitch()
{
    return _pitch;
}

std::optional<float> IMU::get_yaw()
{
    return _yaw;
}

void IMU::init()
{
    Wire.end();
    Wire.begin(IMU_SENSOR_SDA, IMU_SENSOR_SCL, 400000);
    mpu.initialize();

    devStatus = mpu.dmpInitialize();

    auto status = mpu.testConnection();

    if (!status)
    {
        ESP_LOGE(TAG, "Could not find a valid IMU, check wiring!");
    }

    // make sure it worked (returns 0 if so)
    if (devStatus == 0)
    {
        // Calibration Time: generate offsets and calibrate our MPU6050

        mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
        mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
        // mpu.setDLPFMode(MPU6050_DLPF_BW_20);

        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}