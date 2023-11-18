#ifndef VELOHUB_SENSOR_H
#define VELOHUB_SENSOR_H

#include <Arduino.h>
#include <BLEDevice.h>
#include "queue.h"
#include <queue.h>
void ble_task_code(void *parameter);

// Define a struct
struct pinRead
{
    int pin;
    int value;
};

class Sensor : public BLEAdvertisedDeviceCallbacks
{
    QueueHandle_t structQueue;
    char const *name;
    long wait = 0;
    // BLE
    BLEUUID _service_id;        // BLEUUID("0000180d-0000-1000-8000-00805f9b34fb");
    BLEUUID _characteristic_id; // = BLEUUID((uint16_t)0x2A37);
    BLEAddress *server_address = nullptr;
    BLERemoteCharacteristic *characteristic;
    bool doConnect = false;
    bool connected = false;
    bool newHr = false;
    const uint8_t notificationOn[2] = {0x1, 0x0};
    const uint8_t notificationOff[2] = {0x0, 0x0};
    bool connect(BLEAddress address);
    void onResult(BLEAdvertisedDevice device);
    void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

protected:
    uint16_t minv, maxv, avgv, lastv, sumv, count, zonev, lastr;

public:
    Sensor(char const *device_name, size_t buffer_size, BLEUUID service_id, BLEUUID characteristic_id)
    {
        this->name = device_name;
        this->queue = createQueue(buffer_size);
        this->_service_id = service_id;
        this->_characteristic_id = characteristic_id;

        structQueue = xQueueCreate(10, sizeof(struct pinRead) // Queue item size
        );
    }
    virtual ~Sensor()
    {
    }
    virtual void interpret(uint8_t *pData, size_t length);
    struct Queue *queue;
    void init();
    void loop();
    uint16_t last();
    uint16_t min();
    uint16_t max();
    uint16_t avg();
    uint8_t zone();
};

#endif