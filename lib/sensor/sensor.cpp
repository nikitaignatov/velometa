#include "sensor.h"

uint16_t Sensor::last() { return lastv; }
uint16_t Sensor::avg() { return avgv; }
uint16_t Sensor::min() { return minv; }
uint16_t Sensor::max() { return maxv; }
uint8_t Sensor::zone() { return zonev; }

void Sensor::notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    // Serial.printf("\nsensor-reading: %s.\n",name.c_str());

    lastr = millis();
    interpret(pData, length);
    enqueue(queue, lastv);

    if (minv > lastv || minv == 0)
        minv = lastv;
    if (maxv < lastv)
        maxv = lastv;
    sumv += lastv;
    count++;
    avgv = sumv / count;

    Serial.printf("\nsensor %s: ", name);
    Serial.print(lastv);
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

bool Sensor::connect(BLEAddress address)
{
    auto *client = BLEDevice::createClient();
    client->connect(address, esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM);
    Serial.printf("\n-> Connected to %s server.-\n", name);
    auto *svc = client->getService(_service_id);
    if (svc == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(_service_id.toString().c_str());
        return (false);
    }
    else
    {
        Serial.print("Connected to serivce ");
        Serial.println(_service_id.toString().c_str());
    }

    if (client->isConnected())
    {
        Serial.println("connected ");
        characteristic = svc->getCharacteristic(_characteristic_id);
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

void Sensor::onResult(BLEAdvertisedDevice device)
{
    Serial.printf("%s scan result: %s\n", name, device.getName().c_str());
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

void Sensor::init()
{
    if (name == "XXX")
    {
        doConnect = false;
        return;
    }
    Serial.printf("\ninit %s", name);
    BLEDevice::init("");
    auto *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(this);
    pBLEScan->setActiveScan(true);
    Serial.println("init scan");

    pBLEScan->start(5);
    Serial.println("start scan");
}

void Sensor::loop()
{
    long now = millis();

    if (name == "XXX")
    {
        Serial.println("hr loop xxx");
        return;
    }
    if (doConnect == true)
    {
        Serial.printf("\n%s loop connect", name);
        if (connect(*server_address))
        {
            Serial.printf("%s loop connected", name);
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
    if ((now - lastr) > 20e3)
    {
        Serial.printf("\nSensor %s lost connection.\n", name);
        init();
    }
    if (server_address == nullptr)
    {
        init();
    }
}

void Sensor::interpret(uint8_t *pData, size_t length)
{
    printf("SENSRO");
}