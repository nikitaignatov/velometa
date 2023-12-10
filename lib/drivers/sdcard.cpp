#include "sdcard.hpp"
static FATFS fatfs;

FATFS *fs_mount_sd_card()
{
    init_sdspi();

    f_mount(&fatfs, "S:", 1);
    return &fatfs;
}

bool init_sdspi()
{
    sdspi_device_config_t device_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    device_config.host_id = SDSPI_HOST_ID;
    device_config.gpio_cs = SD_CS;
    //device_config.gpio_cd = -1;   // SD Card detect

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

void write_task_code(void *parameter)
{
    Serial.println("write_task_code");

    pinMode(SD_CS, OUTPUT); //SD Card SS
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
    SD.begin(SD_CS);
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    gps_data_t msg;
    for (;;)
    {
        int count = 0;
        std::string lines = "";

        xEventGroupWaitBits(sensor_status_bits, (1 << 1), pdFALSE, pdTRUE, portMAX_DELAY);
        if (xSemaphoreTake(vh_display_semaphore, (TickType_t)20) == pdTRUE)
        {
            while (xQueueReceive(vh_gps_queue, &msg, 10 / portTICK_RATE_MS) == pdPASS)
            {
                if (msg.mocked)
                {
                    continue;
                }
                if (count == 0)
                {
                    lines = fmt::format("{},{},{},{},{}\n", msg.date, msg.time, msg.lat, msg.lon, msg.satelites);
                }
                else
                {
                    lines = fmt::format("{}{},{},{},{},{}\n", lines, msg.date, msg.time, msg.lat, msg.lon, msg.satelites);
                }

                count++;
            }
            if (lines.length() > 1)
            {
                Serial.println("DUMP TO CSV:");
                Serial.println(lines.c_str());
                appendFile(SD, "/gps.csv", lines.c_str());
            }

            xSemaphoreGive(vh_display_semaphore);
            vTaskDelay(60 * 1000 / portTICK_PERIOD_MS);
        }
        else
        {
            ESP_LOGE("metric_info_t", "failed to take semaphor");
        }
    }
}
