#include <Arduino.h>
#include <BLEDevice.h>
#include "queue.h"

class Power : public BLEAdvertisedDeviceCallbacks
{
    String name;
    int interpret(uint8_t *pData, size_t length);

    // BLE
    BLEUUID service_id = BLEUUID("00001818-0000-1000-8000-00805f9b34fb");
    BLEUUID characteristic_id = BLEUUID((uint16_t)0x2A63);
    BLEAddress *server_address;
    BLERemoteCharacteristic *characteristic;
    boolean doConnect = false;
    boolean connected = false;
    boolean new_value = false;
    const uint8_t notificationOn[2] = {0x1, 0x0};
    const uint8_t notificationOff[2] = {0x0, 0x0};
    bool connect(BLEAddress address);
    void onResult(BLEAdvertisedDevice device);
    void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

public:
    Power(String device_name)
    {
        name = device_name;
    }
    struct Queue *queue = createQueue(296 / 2);
    void init();
    void loop();
    int power;
    String contact;
};
