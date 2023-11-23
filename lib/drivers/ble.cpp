#include "ble.hpp"

char const *TAG = "ble";
void notifyCallback(sensor_definition_t sensor, BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
bool connect(sensor_definition_t sensor);
void init_scan();
uint64_t xx_time_get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}
void ble_task_code(void *parameter)
{
    ESP_LOGI(TAG, "begin");
    init_scan();
    ESP_LOGI(TAG, "setup done.");
    for (;;)
    {
        for (auto sensor : ble_sensors)
        {
            ESP_LOGV(TAG, "Check sensor %s. address:%s\n", sensor.device_name, sensor.address.toString().c_str());

            if (sensor.client && sensor.client->isConnected())
            {
                continue;
            }
            else if (!missing_address.equals(sensor.address))
            {
                Serial.printf("Connect to sensor %s\n", sensor.device_name);
                connect(sensor);
            }
            else if (sensor.enabled)
            {
                Serial.printf("Scan enabled sensor %s. address:%s\n", sensor.device_name, sensor.address.toString().c_str());
                init_scan();
            }
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice device)
    {
        for (auto &sensor : ble_sensors)
        {
            if (sensor.client && sensor.client->isConnected())
            {
                continue;
            }
            if (device.haveServiceUUID() && device.isAdvertisingService(sensor.service_id))
            {
                Serial.printf("%s scan result: %s\n", sensor.device_name, device.getName().c_str());
                if (String(device.getName().c_str()).equalsIgnoreCase(String(sensor.device_name)))
                {
                    Serial.println("scan result match");
                    sensor.address = BLEAddress(device.getAddress());
                    sensor.client = BLEDevice::createClient();
                }
            }
        }
    }
};

void init_scan()
{
    Serial.println("init ble scan");
    BLEDevice::init("");
    auto *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    Serial.println("init scan");
    pBLEScan->start(15);
    Serial.println("init done");
}

void notifyCallback(sensor_definition_t sensor, BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
    sensor.parse_data(pData, length);

    Serial.printf("%s[ %s ]\n", sensor.device_name, sensor.address.toString().c_str());
    // Serial.printf("%s[ %s ] payload: [", sensor.device_name, sensor.address.toString().c_str());
    // for (size_t i = 0; i < length; i++)
    // {
    //     Serial.print("0x");
    //     Serial.print(pData[i] < 16 ? "0" : "");
    //     Serial.print(pData[i], HEX);
    //     Serial.print(",");
    // }
    // Serial.println("];");
}

bool connect(sensor_definition_t sensor)
{
    auto address = sensor.address;
    if (missing_address.equals(sensor.address))
    {
        return false;
    }

    auto connected = sensor.client->connect(address, esp_ble_addr_type_t::BLE_ADDR_TYPE_RANDOM);

    Serial.printf("Connection: %d\n", connected);

    auto *svc = sensor.client->getService(sensor.service_id);
    if (svc == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(sensor.service_id.toString().c_str());
        return false;
    }
    else
    {
        Serial.print("Connected to serivce ");
        Serial.println(sensor.service_id.toString().c_str());
    }

    if (sensor.client->isConnected())
    {
        Serial.println("connected ");
        auto characteristic = svc->getCharacteristic(sensor.characteristic_id);
        characteristic->registerForNotify([sensor](BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
                                          { notifyCallback(sensor, pBLERemoteCharacteristic, pData, length, isNotify); });
        return true;
    }
    else
    {
        Serial.println("disconnected ");
    }
    return false;
}

void ble_parse_hr_data(uint8_t *pData, size_t length)
{
    int hr = 0;
    int i = 0;

    uint8_t byte0 = pData[0];
    boolean hrv_uint8 = (byte0 & 1) == 0;
    uint8_t sensor_contact = (byte0 >> 1) & 3;
    if (sensor_contact == 2)
    {
        Serial.println("No contact detected");
    }
    else if (sensor_contact == 3)
    {
        //        contact = "Contact detected";
    }
    else
    {
        Serial.println("Sensor contact not supported");
    }

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
        raw_measurement_msg_t msg = {
            .measurement = measurement_t::heartrate,
            .ts = xx_time_get_time(),
            .value = hr,
            .scale = 1};
        if (vh_raw_measurement_queue)
            xQueueSend(vh_raw_measurement_queue, &msg, 0);
        Serial.printf("HR: %d -- ", hr);
    }
    else
    {
        0;
    }
}

void ble_parse_power_watt_data(uint8_t *pData, size_t length)
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
        printf("power data length should be at least 2, received %d", length);
    }
    if (length > 34)
    {
        printf("power data length should be at most 34, received %d", length);
    }

    // uint16_t flags = (pData[1] << 8) | pData[0];

    uint16_t power = (pData[3] << 8) | pData[2];
    // uint8_t power_balance = pData[4] / 2;
    // int crank = ((pData[6] << 8) | pData[5]);
    // int ct = ((pData[8] << 8) | pData[7]) / 1024;

    if (power >= 0)
    {
        raw_measurement_msg_t msg = {
            .measurement = measurement_t::power,
            .ts = xx_time_get_time(),
            .value = power,
            .scale = 1};
        // if (vh_raw_measurement_queue)
        xQueueSend(vh_raw_measurement_queue, &msg, 0);
        Serial.printf("POWER: %d -- ", power);
    }
}

bool IsBitSet(byte b, int pos)
{
    return ((b >> pos) & 1) != 0;
}

static int last_tv, last_rv;
void ble_parse_speed_wheel_rpm_data(uint8_t *pData, size_t length)
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

        uint16_t speed = ((((cdiff * 2105) / 1.e6f) / ((float_t)d / 3.6e6f))) * 100;
        if (speed >= 0 && speed < 10000) // TODO: implement propper handling of edgecases.
        {
            raw_measurement_msg_t msg = {
                .measurement = measurement_t::speed,
                .ts = xx_time_get_time(),
                .value = speed,
                .scale = 100};
            if (vh_raw_measurement_queue)
                xQueueSend(vh_raw_measurement_queue, &msg, 0);
            Serial.printf("SPEED: %d -- ", speed);
        }
    }
    else
    {
    }
}