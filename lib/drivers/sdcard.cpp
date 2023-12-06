#include "sdcard.hpp"
FATFS fatfs;

FATFS *fs_mount_sd_card()
{
    init_sdspi();

    f_mount(&fatfs, "S:", 1);
    // lv_fs_file_t f;
    // lv_fs_res_t res = lv_fs_open(&f, "S:20478.png", LV_FS_MODE_RD);
    // Serial.printf("res: %d\n", res);
    // res = lv_fs_open(&f, "S:/20478.png", LV_FS_MODE_RD);
    // Serial.printf("res: %d\n", res);
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