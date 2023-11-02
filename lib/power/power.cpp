#include "power.h"

int Power::last() { return lastv; }
int Power::avg() { return avgv; }
int Power::min() { return minv; }
int Power::max() { return maxv; }
int Power::zone() { return zonev; }

int Power::interpret(uint8_t *pData, size_t length)
{
    // Configure the Cycle Power Measurement characteristic

    // Properties = Notify
    // Min Len    = 2
    // Max Len    = 34
    //    B0:1    = UINT16  - Flag (MANDATORY)
    //      b0    = Pedal power balance present; 0 = false, 1 = true
    //      b1    = Pedal power balance reference; 0 = unknown, 1 = left
    //      b2    = Accumulated torque present; 0 = false, 1 = true
    //      b3    = Accumulated torque source; 0 = wheel, 1 = crank
    //      b4    = Wheel revolution data present; 0 = false, 1 = true
    //      b5    = Crank revolution data present; 0 = false, 1 = true
    //      b6    = Extreme force magnatudes present; 0 = false, 1 = true
    //      b7    = Extreme torque magnatues present; 0 = false, 1 = true
    //      b8    = Extreme angles present; 0 = false, 1 = true
    //      b9    = Top dead angle present; 0 = false, 1 = true
    //      b10   = Bottom dead angle present; 0 = false, 1 = true
    //      b11   = Accumulated energy present; 0 = false, 1 = true
    //      b12   = Offset compensation indicator; 0 = false, 1 = true
    //      b13   = Reseved
    //      b14   = n/a
    //      b15   = n/a
    //    B2:3    = SINT16 - Instataineous power, Watts (decimal)
    //    B4      = UINT8 -  Pedal power balance, Percent (binary) 1/2
    //    B5:6    = UINT16 - Accumulated torque, Nm; res (binary) 1/32
    //    B7:10   = UINT32 - Cumulative wheel revolutions, (decimal)
    //    B11:12  = UINT16 - Last wheel event time, second (binary) 1/2048
    //    B13:14  = UINT16 - Cumulative crank revolutions, (decimal)
    //    B15:16  = UINT16 - Last crank event time, second (binary) 1/1024
    //    B17:18  = SINT16 - Max force magnitude, Newton (decimal)
    //    B19:20  = SINT16 - Min force magnitude, Newton (decimal)
    //    B21:22  = SINT16 - Max torque magnitude, Nm (binary) 1/1024
    //    B23:24  = SINT16 - Min torque magnitude, Nm (binary) 1/1024
    //    B25:26  = UINT12 - Max angle, degree (decimal)
    //    B27:28  = UINT12 - Min angle, degree (decimal)
    //    B29:30  = UINT16 - Top dead spot angle, degree (decimal)
    //    B31:32  = UINT16 - Bottom dead spot angle, degree (decimal)
    //    B33:34  = UINT16 - Accumulated energy, kJ (decimal)

    if (length < 2)
    {
        printf("power data length should be at least 2, received %lu", length);
    }
    if (length > 34)
    {
        printf("power data length should be at most 34, received %lu", length);
    }

    uint16_t flags = (pData[1] << 8) | pData[0];

    int power = (pData[3] << 8) | pData[2];
    uint8_t power_balance = pData[4] / 2;
    int crank = ((pData[6] << 8) | pData[5]);
    int ct = ((pData[8] << 8) | pData[7]) / 1024;

    if (power > 0)
    {
        lastv = power;
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

    // printf("%u\t%d\t%d\t%d\t%d\n", flags, power, power_balance, crank, ct);
    return lastv;
}

int calculate_zone(int power)
{
    float_t ftp = (float_t)FTP;
    int pct = power / ftp * 100;
    int zone = 0;
    if (POWER_Z1_MIN < pct && POWER_Z1_MAX > pct) zone = 1;
    else if (POWER_Z2_MIN < pct && POWER_Z2_MAX > pct) zone = 2;
    else if (POWER_Z3_MIN < pct && POWER_Z3_MAX > pct) zone = 3;
    else if (POWER_Z4_MIN < pct && POWER_Z4_MAX > pct) zone = 4;
    else if (POWER_Z5_MIN < pct && POWER_Z5_MAX > pct) zone = 5;
    else if (POWER_Z6_MIN < pct && POWER_Z6_MAX > pct) zone = 6;
    else if (POWER_Z7_MIN < pct) zone = 6;
    return zone;
}

void Power::notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    new_value = true;
    power = interpret(pData, length);
    zonev = calculate_zone(lastv);
    return;

    Serial.print("\npower: ");
    Serial.print(power);
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

bool Power::connect(BLEAddress address)
{
    auto *client = BLEDevice::createClient();
    client->connect(address, esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM);
    Serial.println(" - Connected to power server");
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
    return true;
}

void Power::onResult(BLEAdvertisedDevice device)
{
    Serial.printf("scan result: %s\n", device.getName());
    if (String(device.getName().c_str()).equalsIgnoreCase(name))
    {
        Serial.println("scan result match");
        device.getScan()->stop();
        server_address = new BLEAddress(device.getAddress());
        doConnect = true;
        Serial.println("Device found. Connecting!");
    }
}

void Power::init()
{
    Serial.println("init power");
    BLEDevice::init("");
    auto *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(this);
    pBLEScan->setActiveScan(true);
    Serial.println("init scan");

    pBLEScan->start(5);
    Serial.println("start scan");

    // uint8_t list[][9] = {
    //     {0x23, 0x00, 0x0A, 0x00, 0xC8, 0x03, 0x00, 0x0F, 0x17},
    //     {0x23, 0x00, 0x0C, 0x00, 0xC8, 0x15, 0x00, 0xC5, 0x7A},
    //     {0x23, 0x00, 0x0A, 0x00, 0xC8, 0x16, 0x00, 0xD6, 0x82},
    //     {0x23, 0x00, 0x23, 0x00, 0x62, 0x1B, 0x00, 0xFF, 0xA7},
    //     {0x23, 0x00, 0x0F, 0x00, 0xC8, 0x1A, 0x00, 0xBE, 0xA2},
    //     {0x23, 0x00, 0x0A, 0x00, 0xC8, 0x1C, 0x00, 0xD2, 0xAE},
    //     {0x23, 0x00, 0x3F, 0x01, 0x72, 0x12, 0x00, 0x1A, 0x3B},
    //     {0x23, 0x00, 0x3A, 0x05, 0x78, 0x0D, 0x00, 0x98, 0x2E},
    //     {0x23, 0x00, 0xff, 0x00, 0xA6, 0x0F, 0x00, 0x70, 0x33}};

    // printf("Flags\tPower\tBalance\tRev\tTime\n");
    // for (int i = 0; i < 9; i++)
    // {
    //     int power = interpret(list[i], 9);
    // }
}


void Power::loop()
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
    if (new_value)
    {
        new_value = false;
        Serial.println("got power data");
    }
    long now = millis();
    if (wait - now < 0)
    {
        wait = now + 10e3;
        init();
    }
    if (server_address == nullptr)
    {
        init();
    }
}
