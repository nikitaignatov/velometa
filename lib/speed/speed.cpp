#include "speed.h"
// 01-4F-00-00-00-F7-3B
// 01-5B-00-00-00-5C-5E
// 01-62-00-00-00-11-75
// 01-69-00-00-00-95-8E
// 0x01 0x02 0x01 0x00 0x00 0x49 0x8
// 0x01 0x03 0x01 0x00 0x00 0x9F 0x8D
// 0x01 0x05 0x01 0x00 0x00 0xA8 0x98
// 0x01 0x13 0x01 0x00 0x00 0x94 0x08

// 0x01 0x48 0x01 0x00 0x00 0x6F 0x10
bool IsBitSet(byte b, int pos)
{
    return ((b >> pos) & 1) != 0;
}
int Speed::interpret(uint8_t *pData, size_t length)
{
    uint32_t revs_acc = 0;
    uint16_t lwet = 0;

    uint8_t byte0 = pData[0];
    boolean cwr_uint8 = IsBitSet(byte0, 0);
    boolean lwet_uint8 = IsBitSet(byte0, 0);

    revs_acc = (pData[4] << 24) | (pData[3] << 16) | (pData[2] << 8) | pData[1];
    lwet = (pData[6] << 8) | pData[5];

    if (revs_acc > 0 && revs_acc - last_rv > 0)
    {
        // r:673 p:672 cdiff:1 w:2105 d:677 r: 3
        int p = last_rv;
        int t = last_tv;
        last_rv = revs_acc;
        last_tv = lwet;
        int d = (lwet - t);
        d = d == 0 ? 1 : d;
        int cdiff = (revs_acc - p);
        printf("r:%d p:%d cdiff:%d w:%d d:%d r: %f\n", last_rv, p, cdiff, cdiff * 2105, d, ((cdiff * 2105) / 100.f) / ((float_t)d / 1000.0f));

        lastv = (((cdiff * 2105) / 1.e6f) / ((float_t)d / 3.6e6f));

        lastr = millis();

        if (minv > lastv || minv == 0)
            minv = lastv;
        if (maxv < lastv)
            maxv = lastv;
        sumv += lastv;
        count++;
        avgv = sumv / count;
        enqueue(queue, lastv);
    }
    else
    {
        lastv = 0;
    }

    return lastv;
}

int Speed::last() { return lastv; }
int Speed::avg() { return avgv; }
int Speed::min() { return minv; }
int Speed::max() { return maxv; }

void Speed::notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    newHr = true;

    int speed = interpret(pData, length);

    Serial.print("\nspeed: ");
    Serial.print(speed);
    Serial.print("; payload: ");
    for (size_t i = 0; i < length; i++)
    {
        Serial.print("0x");
        Serial.print(pData[i] < 16 ? "0" : "");
        Serial.print(pData[i], HEX);
        Serial.print(" ");
    }
    Serial.println(";");
}

bool Speed::connect(BLEAddress address)
{
    auto *client = BLEDevice::createClient();
    client->connect(address, esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM);
    Serial.println(" - Connected to speed server");
    auto *svc = client->getService(service_id);
    if (svc == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(service_id.toString().c_str());
        return (false);
    }
    else
    {
        Serial.print("Connected to serivce ");
        Serial.println(service_id.toString().c_str());
    }

    if (client->isConnected())
    {
        Serial.print("connected ");
        characteristic = svc->getCharacteristic(characteristic_id);
    }
    else
    {
        Serial.print("disconnected ");
    }
    if (characteristic == nullptr)
    {
        Serial.print("Failed to find our characteristic UUID");
        return false;
    }
    Serial.println(" - Found our characteristics");

    characteristic->registerForNotify([this](BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
                                      { this->notifyCallback(pBLERemoteCharacteristic, pData, length, isNotify); });
    lastr = millis();
    return true;
}

void Speed::onResult(BLEAdvertisedDevice device)
{
    Serial.printf("scan result: %s\n", device.getName());
    if (String(device.getName().c_str()).equalsIgnoreCase(name))
    {
        Serial.println("scan result match");
        device.getScan()->stop();
        server_address = new BLEAddress(device.getAddress());
        doConnect = true;
        Serial.println(server_address->toString().c_str());
        Serial.println("Device found. Connecting!");
    }
}

void Speed::init()
{
    if (name == "XXX")
    {
        doConnect = false;
        return;
    }
    Serial.println("init speed sensor");
    BLEDevice::init("");
    auto *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(this);
    pBLEScan->setActiveScan(true);
    Serial.println("init scan");

    pBLEScan->start(15);
    Serial.println("start scan");
}


void Speed::loop()
{
    if (doConnect == true)
    {
        if (connect(*server_address))
        {
            Serial.println("We are now connected to the BLE Server.");
            characteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)notificationOn, 2, true);
            connected = true;
        }
        else
        {
            Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
        }
        doConnect = false;
    }
    if (newHr)
    {
        newHr = false;
        Serial.println("got speed data");
    }
    long now = millis();
    if (wait - now < 0)
    {
        wait = now + 60e3;
        init();
    }
    if (server_address == nullptr)
    {
        init();
    }
}