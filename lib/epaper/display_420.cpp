#include "display_420.hpp"

GxEPD2_BW<SCREEN, SCREEN::HEIGHT> display(SCREEN(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)
const int SCRN_SPI_CHAN = 2; // HSPI
SPIClass hspi(HSPI);

void clear_screen()
{
    display.fillScreen(GxEPD_WHITE);
}

void display_layout()
{
    display.setRotation(0);
    // status
    display.drawFastHLine(0, 20, SCREEN::WIDTH, GxEPD_BLACK);
    display.drawFastVLine(85, 0, 20, GxEPD_BLACK);

    // sensor
    display.drawFastHLine(0, 60, SCREEN::WIDTH, GxEPD_BLACK);
    display.drawFastHLine(0, 140, SCREEN::WIDTH, GxEPD_BLACK);
    display.drawFastVLine(200, 0, 60, GxEPD_BLACK);
    display.drawFastVLine(200, 21, 39, GxEPD_WHITE);

    // map
    // display.drawFastVLine(0, 140, 100, GxEPD_BLACK);
    // display.drawFastVLine(display.epd2.WIDTH - 1, 140, 100, GxEPD_BLACK);
    // display.drawFastHLine(0, 240, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);

    // display.drawFastHLine(0, GxEPD2_420_M01::HEIGHT - 20, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
}

void display_time(int secs)
{
    int min = secs / 60;
    int hour = min / 60;
    int m = min % 60;
    int s = secs % 60;
    int h = hour;
    char temp_str[20];

    sprintf(temp_str, "%01d:%02d:%02d\0", h, m, s);
    display.setCursor(4, 12);
    display.print(temp_str);
}

void display_bottom(float_t h, float_t s, float_t lat, float_t lon)
{
    int y = 280;

    display.setCursor(0, y);
    display.setFont(&FreeMono9pt7b);
    display.setTextColor(GxEPD_BLACK);

    display.print(h, 0);
    display.setCursor(50, y);
    display.print(s, 1);
    display.setCursor(200, y);
    display.print(lat, 4);
    display.setCursor(300, y);
    display.print(lon, 4);
}

void display_last_hr(uint16_t hr)
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.setFont(&FreeMonoBold24pt7b);
    display.getTextBounds(String(hr).c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor((400 - tbw) - 8, 55);
    display.print(String(hr).c_str());
}

void display_last_power(uint16_t power)
{
    display.setFont(&FreeMonoBold24pt7b);
    display.setCursor(0, 55);
    display.print(String(power).c_str());
}

void display_zone_hr(uint8_t zone)
{
    display.fillRect(200, 20, 30, 40, GxEPD_BLACK);
    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(202, 55);
    display.print(String(zone).c_str());
}

void display_zone_power(uint8_t zone)
{
    display.fillRect(170, 20, 30, 40, GxEPD_BLACK);
    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(170, 55);
    display.print(String(zone).c_str());
}

void show()
{
    display.display(true);
}

void display_status_bar_content(int secs)
{
    display.setFont(&FreeMono9pt7b);
    display_time(secs);
}

void display_chart(Queue *queue, int screen_x, int screen_y, int chart_height)
{
    if (queue->size > 0)
    {
        int padding = 2;
        int screen_height = screen_y - padding;
        int min = queue->min;
        int range = queue->max - min;
        range = range == 0 ? 1 : range;
        float_t scaled = (front(queue) - min) / (float_t)range;
        int height = chart_height - (padding * 2);

        int p = screen_height - scaled * height;
        for (size_t i = 0; i < queue->size; i++)
        {
            int s = (queue->front + i) % queue->capacity;
            int c = queue->array[s];
            float_t scaled = ((float_t)c - (float_t)min) / (float_t)range;
            int y = screen_height - scaled * height;

            int xx = i + screen_x;
            display.drawLine(xx, p, xx + 1, y, GxEPD_BLACK);
            p = y;
        }

        display.setTextColor(GxEPD_BLACK);
        display.fillRect(0, screen_y - (height + 8), 100, 16, GxEPD_WHITE);
        display.setFont(&FreeMono9pt7b);
        display.setCursor(screen_x, screen_y - height + 8);
        display.printf("%d %d %d", queue->min, queue->max, queue->avg);
    }
}

void render(int secs, HR *hr, Power *power, Speed *speed)
{
        clear_screen();
        display.setTextColor(GxEPD_BLACK);

        // int a = system_bar_h + Font12.Height + Font24.Height;
        display_status_bar_content(secs);
        display.setCursor(90, 12);
        display.print(String(String(speed->last()) + String("km/h")).c_str());

        display_last_hr(hr->last());
        display_last_power(power->last());
        display_zone_hr(calculate_hr_zone(hr->last()));
        display_zone_power(calculate_power_zone(power->last(), FTP));

        display_chart(power->queue, 0, 140, 80);
        display_chart(hr->queue, 0, 220, 80);
        display_layout();
}

void refresh_screen()
{
    clear_screen();
    display.display();
}

void partial_update(String k)
{
    display.setFullWindow();
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 50);
    display.print(k.c_str());
    display.display(true); // partial update
    Serial.println("partial_update done");
}

void display_init()
{
    hspi.begin(EPD_SCK_PIN, 12, EPD_MOSI_PIN, 15);                                         // remap hspi for EPD (swap pins)
    display.init(115200, true, 2, false, hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0)); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
}



#if USE_EPAPER
void display_task_code(void *parameter)
{
    Serial.println("display_task_code");
    long last = 0;
    uint8_t refresh = 0;

    datafield_t hr_f = {
        .type = datafield_type_t::chart,
        .label = "HR Zone",
    };

    page_t dash = {
        .datafields = {
            hr_f,
        },
    };

    screen_t screen = {
        .pages = {
            dash,
        },
    };

    for (;;)
    {
        long secs = millis();
        if (refresh)
        {
            refresh_screen();
            refresh = false;
        }
        render(secs / 1000, &hr_monitor, &power_monitor, &speed_monitor);
        // // display_bottom(height, speed, lat, lon);
        show();
        // for (auto page : screen.pages)
        // {
        //     Serial.println("Page");
        //     for (auto field : page.datafields)
        //     {
        //         Serial.printf("Field: %s", field.label.c_str());
        //         Serial.println(".");
        //     }
        // }
    }
}
#endif
