#include "display_420.h"

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(GxEPD2_420_M01(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)
const int SCRN_SPI_CHAN = 2;                                                                                                           // HSPI
SPIClass hspi(HSPI);

void display_layout()
{
    display.setRotation(0);
    // status
    display.drawFastHLine(0, 20, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
    display.drawFastVLine(85, 0, 20, GxEPD_BLACK);

    // sensor
    display.drawFastHLine(0, 60, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
    display.drawFastHLine(0, 140, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
    display.drawFastVLine(200, 0, 140, GxEPD_BLACK);

    // map
    display.drawFastVLine(0, 140, 100, GxEPD_BLACK);
    display.drawFastVLine(display.epd2.WIDTH - 1, 140, 100, GxEPD_BLACK);
    display.drawFastHLine(0, 240, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);

    display.drawFastHLine(0, GxEPD2_420_M01::HEIGHT - 20, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
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


void display_status_bar_content(int secs)
{
    display.setFont(&FreeMono9pt7b);
    display_time(secs);
}


void display_chart(Queue *queue, int x)
{
    if (queue->size > 0)
    {
        int screen_height = 140;
        int min = queue->min;
        int range = queue->max - min;
        range = range == 0 ? 1 : range;
        float_t scaled = (front(queue) - min) / (float_t)range;
        int height = 80;

        int p = screen_height - scaled * height;
        for (size_t i = 0; i < queue->size; i++)
        {
            int s = (queue->front + i) % queue->capacity;
            int c = queue->array[s];

            float_t scaled = ((float_t)c - (float_t)min) / (float_t)range;
            int y = screen_height - scaled * height;

            int xx = i + x;
            display.drawLine(xx,p,xx+1,y,GxEPD_BLACK);
            p = y;
        }
    }
}

void display_map_dark(int16_t offset_x, int16_t offset_y)
{
    int y_offset = 120;
    if (offset_x >= 0)
    {
        display.fillTriangle(190, y_offset + 78, 200, y_offset + 50, 210, y_offset + 78, GxEPD_WHITE);
        display.epd2.drawImagePart(display_map_tile_1, offset_x, offset_y, map_w, map_h, 0, (int16_t)142, (int16_t)400, (int16_t)140, false, false, false);
        // display.drawBitmap(0, 140+offset_x, display_map_tile_1, map_w, map_h, GxEPD_BLACK);
        display.refresh(true);
    }
    else
    {
    }
}

void render(int secs, HR *hr, Power *power, Speed *speed, int offset_x)
{
    // // clear();
    display.setPartialWindow(0, 0, 400, 142);

    display.fillScreen(GxEPD_WHITE);
    // // int a = system_bar_h + Font12.Height + Font24.Height;
    display_status_bar_content(secs);
    display.setCursor(90, 12);
    display.print(String(String(speed->last())+String("km/h")).c_str());
    display.setCursor(0, 55);

    display.setFont(&FreeMonoBold24pt7b);
    display.print(String(hr->last()).c_str());
    display.setCursor(202, 55);
    display.print(String(power->last()).c_str());
    // display.print(String(198).c_str());

    display.setFont(&FreeMonoBold9pt7b);
    display_chart(hr->queue,0);
    display_chart(power->queue,201);
    display_layout();
    display.displayWindow(0, 0, 400, 142);

    // display.display(true);
    // display_map_dark(offset_x);

    // Paint_DrawLine(0, a - 1, screen_width, a - 1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // Paint_DrawLine(0, system_bar_h - 1, screen_width, system_bar_h - 1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // Paint_DrawLine(cell_width, system_bar_h, cell_width, screen_height, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // display_time(secs);
    // display_hr(hr);
    // display_power(power);

    // Paint_DrawString_EN(230, 0, String(speed->last()).c_str(), &Font20, WHITE, BLACK);
    // Paint_DrawString_EN(230 + (Font20.Width * 2), 4, String("km/h").c_str(), &Font12, WHITE, BLACK);
    // EPD_2IN9_Display(BlackImage);
}
void refresh_screen()
{
    display.fillScreen(GxEPD_WHITE);
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



void display_task_code(void *parameter)
{
    Serial.println("display_task_code");
    long last = 0;
    for (;;)
    {
        long secs = millis();
        if (refresh)
        {
            // refresh_screen();
            refresh = false;
        }
        render(secs / 1000, &hr_monitor, &power_monitor, &speed_monitor, counter);
    }
}