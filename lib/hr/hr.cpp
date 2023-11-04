#include "hr.h"

int HR::interpret(uint8_t *pData, size_t length)
{
    int hr = 0;
    int i = 0;
    contact = "";

    uint8_t byte0 = pData[0];
    boolean hrv_uint8 = (byte0 & 1) == 0;
    uint8_t sensor_contact = (byte0 >> 1) & 3;
    if (sensor_contact == 2)
    {
        contact = "No contact detected";
        Serial.println(contact);
    }
    else if (sensor_contact == 3)
    {
        contact = "Contact detected";
    }
    else
    {
        contact = "Sensor contact not supported";
        Serial.println(contact);
    }

    uint8_t ee_status = ((byte0 >> 3) & 1) == 1;
    uint8_t rr_interval = ((byte0 >> 4) & 1) == 1;

    if (hrv_uint8)
    {
        hr = pData[1];
        i = 2;
    }
    else
    {
        hr = (pData[2] << 8) | pData[1];
        i = 3;
    }

    if (hr > 0)
    {
        lastv = hr;
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

    if (HR_Z1_MIN < lastv && HR_Z1_MAX > lastv)
        zonev = 1;
    if (HR_Z2_MIN < lastv && HR_Z2_MAX > lastv)
        zonev = 2;
    if (HR_Z3_MIN < lastv && HR_Z3_MAX > lastv)
        zonev = 3;
    if (HR_Z4_MIN < lastv && HR_Z4_MAX > lastv)
        zonev = 4;
    if (HR_Z5_MIN < lastv && HR_Z5_MAX > lastv)
        zonev = 5;
    return hr;
}

int HR::last() { return lastv; }
int HR::avg() { return avgv; }
int HR::min() { return minv; }
int HR::max() { return maxv; }
int HR::zone() { return zonev; }

void HR::notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    newHr = true;

    heart_rate = interpret(pData, length);
    return;

    Serial.print("\nhr: ");
    Serial.print(heart_rate);
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

bool HR::connect(BLEAddress address)
{
    auto *client = BLEDevice::createClient();
    client->connect(address, esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM);
    Serial.println(" - Connected to hr server");
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

void HR::onResult(BLEAdvertisedDevice device)
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

void HR::init()
{
    Serial.println("init hr");
    BLEDevice::init("");
    auto *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(this);
    pBLEScan->setActiveScan(true);
    Serial.println("init scan");

    pBLEScan->start(15);
    Serial.println("start scan");
}

void HR::loop()
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
        Serial.println("got hr data");
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