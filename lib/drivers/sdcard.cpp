#include "sdcard.hpp"
extern FATFS fatfs;
static const char *TAG = "write_task_code";

FATFS *fs_mount_sd_card()
{
    init_sdspi();

    pinMode(SD_CS, OUTPUT); // SD Card SS
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    SD.begin(SD_CS);

    f_mount(&fatfs, "S:", 1);
    return &fatfs;
}

bool init_sdspi()
{
    sdspi_device_config_t device_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    device_config.host_id = SDSPI_HOST_ID;
    device_config.gpio_cs = SD_CS;
    // device_config.gpio_cd = -1;   // SD Card detect

    ESP_LOGI(TAG, "Initializing SD card");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = device_config.host_id;

    esp_vfs_fat_mount_config_t mount_config =
        {
            //.format_if_mount_failed = true,
            .format_if_mount_failed = false,
            .max_files = 5,
            .allocation_unit_size = 32 * 1024};

    ESP_LOGI(TAG, "Initializing SPI BUS");
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_MOSI,
        .miso_io_num = SD_MISO,
        .sclk_io_num = SD_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4092,
    };
    esp_err_t ret = spi_bus_initialize(SDSPI_HOST_ID, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &device_config, &mount_config, &sdcard);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. \n"
                          "If you want the card to be formatted, enable above in mount_config.");
            return ESP_FAIL;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). \n"
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
            return ESP_FAIL;
        }
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, sdcard);

    return ESP_OK;
}

static void appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);
    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
}

void write_gps_task_code(void *parameter)
{
    ESP_LOGI(TAG, "write_gps_task_code start");

    gps_data_t msg;

    auto delay_sec = 10;
    auto count_limit = delay_sec * 12;
    for (;;)
    {
        int count = 0;
        std::string lines = "";

        ESP_LOGW(TAG, "GPS CSV LOOP");
        while (xQueueReceive(vh_gps_csv_queue, &msg, 0) == pdPASS && count <= count_limit)
        {
            if (msg.mocked)
            {
                continue;
            }
            lines = fmt::format("{}\n{},{},{},{},{},{},{},{},{},{},{}", lines, msg.tick_ms, msg.lat, msg.lon, msg.satelites, msg.hdop, msg.vdop, msg.has_fix, msg.speed, msg.distance, msg.heading, msg.height);
            count++;
        }
        if (lines.length() > 1)
        {
            ESP_LOGI(TAG, "Dump %d lines size:%d to gps.csv", count, lines.length());
            // ESP_LOGI(TAG, "Data:\n %s", lines.c_str());

            if (xSemaphoreTake(vm_sdcard_semaphor, (TickType_t)200) == pdTRUE)
            {
                appendFile(SD, "/gps.csv", lines.c_str());
                xSemaphoreGive(vm_sdcard_semaphor);
            }
            else
            {
                ESP_LOGD("gps_data_t", "failed to take semaphor");
            }
        }
        else
        {
            ESP_LOGW(TAG, "NO GPS DATA for gps.csv");
        }

        vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
    }
}

void write_measurements_task_code(void *parameter)
{
    ESP_LOGI(TAG, "write_measurements_task_code start");

    raw_measurement_msg_t msg;

    auto delay_sec = 13;
    auto count_limit = 512;
    std::string lines = "";
    for (;;)
    {
        int count = 0;
        lines = "";

        ESP_LOGW(TAG, "MEASUREMENTS CSV LOOP");
        while (count <= count_limit && xQueueReceive(vm_csv_queue, &msg, 1000) == pdPASS)
        {
            if (msg.value && msg.ts > 0)
            {
                switch (msg.measurement)
                {
                case measurement_t::heartrate:
                case measurement_t::power:
                case measurement_t::position_mm:
                    lines = fmt::format("{}\n{},{},{:.0f}", lines, msg.ts, msg.measurement, msg.value);
                    break;
                case measurement_t::elevation:
                    lines = fmt::format("{}\n{},{},{:.1f}", lines, msg.ts, msg.measurement, msg.value);
                    break;
                case measurement_t::air_humidity:
                case measurement_t::air_temperature:
                case measurement_t::air_pressure_abs:
                case measurement_t::air_speed:
                case measurement_t::speed:
                    lines = fmt::format("{}\n{},{},{:.2f}", lines, msg.ts, msg.measurement, msg.value);
                    break;
                case measurement_t::roll:
                case measurement_t::pitch:
                case measurement_t::yaw:
                case measurement_t::diff_pressure_l_pa:
                case measurement_t::diff_pressure_r_pa:
                    lines = fmt::format("{}\n{},{},{:.3f}", lines, msg.ts, msg.measurement, msg.value);
                    break;
                default:
                    lines = fmt::format("{}\n{},{},{}", lines, msg.ts, msg.measurement, msg.value);
                }
                count++;
            }
        }

        if (lines.length() > 1)
        {
            ESP_LOGW(TAG, "Dump lines[ %d ] to measurements.csv", count);
            // ESP_LOGI(TAG, "Data:\n %s", lines.c_str());

            if (xSemaphoreTake(vm_sdcard_semaphor, (TickType_t)500) == pdTRUE)
            {
                appendFile(SD, "/measurements.csv", lines.c_str());
                xSemaphoreGive(vm_sdcard_semaphor);
            }
            else
            {
                ESP_LOGD("gps_data_t", "failed to take semaphor");
            }
        }
        else
        {
            ESP_LOGW(TAG, "NO DATA for measurements.csv");
        }
    }
}

void write_task_code(void *parameter)
{
    ESP_LOGI(TAG, "write_task_code start");

    auto fatfs = fs_mount_sd_card();
    ESP_LOGI(TAG, "fs_mount_sd_card %d init_sdspi", fatfs->fsize);

    pinMode(SD_CS, OUTPUT); // SD Card SS
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    SD.begin(SD_CS);
    ESP_LOGI(TAG, "SD.begin done");

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        ESP_LOGE(TAG, "No SD card attached");

        return;
    }

    ESP_LOGW(TAG, "SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        ESP_LOGW(TAG, "MMC");
    }
    else if (cardType == CARD_SD)
    {
        ESP_LOGW(TAG, "SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        ESP_LOGW(TAG, "SDHC");
    }
    else
    {
        ESP_LOGW(TAG, "UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    ESP_LOGW(TAG, "SD Card Size: %lluMB\n", cardSize);

    xTaskCreatePinnedToCore(
        write_gps_task_code, /* Function to implement the task */
        "write_gps_task",    /* Name of the task */
        8 * 1024,            /* Stack size in words */
        NULL,                /* Task input parameter */
        0,                   /* Priority of the task */
        NULL,                /* Task handle. */
        0);                  /* Core where the task should run */

    xTaskCreatePinnedToCore(
        write_measurements_task_code, /* Function to implement the task */
        "write_measurements_task",    /* Name of the task */
        8 * 1024,                     /* Stack size in words */
        NULL,                         /* Task input parameter */
        0,                            /* Priority of the task */
        NULL,                         /* Task handle. */
        0);                           /* Core where the task should run */

    vTaskDelete(NULL);
}