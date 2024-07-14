#include "imu.hpp"
#define OUTPUT_READABLE_YAWPITCHROLL

static const char *TAG = "environmental_sensor";
bool IMU::read_sensor()
{
    if (dmpReady)
    {
        if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
        {
            mpu.dmpGetQuaternion(&q, fifoBuffer);

            auto q0 = q.w;
            auto q1 = q.x;
            auto q2 = q.y;
            auto q3 = q.z;
            // ESP_LOGW("Q", "%.3f %.3f %.3f", q1, q2, q3);

            auto yr = -atan2(-2 * q1 * q2 + 2 * q0 * q3, q2 * q2 - q3 * q3 - q1 * q1 + q0 * q0);
            auto pr = asin(2 * q2 * q3 + 2 * q0 * q1);
            auto rr = atan2(-2 * q1 * q3 + 2 * q0 * q2, q3 * q3 - q2 * q2 - q1 * q1 + q0 * q0);
            // ESP_LOGW("YPR_R", "%.3f %.3f %.3f", yr, pr, rr);

            _yaw =  yr * 180 / M_PI;
            _pitch = pr * 180 / M_PI;
            _roll = rr * 180 / M_PI;
            // ESP_LOGW("YPR_D", "%.3f %.3f %.3f", _yaw.value(), _pitch.value(), _roll.value());

            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            // _yaw = ypr[0] * 180 / M_PI;
            // _pitch = ypr[1] * 180 / M_PI;
            // _roll = ypr[2] * 180 / M_PI;
            // ESP_LOGW("IMU", "%.3f %.3f %.3f", _yaw, _pitch, _roll);
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