#ifndef VELOHUB_SENSOR_H
#define VELOHUB_SENSOR_H

#include <Arduino.h>
#include <BLEDevice.h>
#include "queue.h"

class Sensor : public BLEAdvertisedDeviceCallbacks
{
    String name;
    long wait = 0;
    // BLE
    BLEUUID _service_id;        // BLEUUID("0000180d-0000-1000-8000-00805f9b34fb");
    BLEUUID _characteristic_id; // = BLEUUID((uint16_t)0x2A37);
    BLEAddress *server_address = nullptr;
    BLERemoteCharacteristic *characteristic;
    boolean doConnect = false;
    boolean connected = false;
    boolean newHr = false;
    const uint8_t notificationOn[2] = {0x1, 0x0};
    const uint8_t notificationOff[2] = {0x0, 0x0};
    bool connect(BLEAddress address);
    void onResult(BLEAdvertisedDevice device);
    void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
protected:
    int minv, maxv, avgv, lastv, sumv, count, zonev, lastr;

public:
    Sensor(String device_name, size_t buffer_size, BLEUUID service_id, BLEUUID characteristic_id)
    {
        this->name = device_name;
        this->queue = createQueue(buffer_size);
        this->_service_id = service_id;
        this->_characteristic_id = characteristic_id;
    }
    virtual ~Sensor()
    {
    }
    virtual void interpret(uint8_t *pData, size_t length);
    struct Queue *queue;
    void init();
    void loop();
    int last();
    int min();
    int max();
    int avg();
    int zone();
};

#endif