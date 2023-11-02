#ifndef SPEED_H
#define SPEED_H

#include <Arduino.h>
#include <BLEDevice.h>
#include "zones.h"
#include "queue.h"

class Speed : public BLEAdvertisedDeviceCallbacks
{
    String name;
    int minv, maxv, avgv, lastv, sumv, count, lastr, last_tv, last_rv;
    int interpret(uint8_t *pData, size_t length);
long wait = 0;
    // BLE
    BLEUUID service_id = BLEUUID("00001816-0000-1000-8000-00805f9b34fb");
    BLEUUID characteristic_id = BLEUUID((uint16_t)0x2A5B);
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

public:
    Speed(String device_name)
    {
        name = device_name;
    }
    struct Queue *queue = createQueue(296 / 2 - 2);
    void init();
    void loop();
    int last();
    int min();
    int max();
    int avg();
};

#endif