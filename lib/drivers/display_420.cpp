#include "display_420.hpp"

#ifdef EPD_75bwr
GxEPD2_3C<SCREEN, SCREEN::HEIGHT / 24> display(SCREEN(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)

#endif

#ifdef EPD_213bw
GxEPD2_BW<SCREEN, SCREEN::HEIGHT> display(SCREEN(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)
#endif

#ifdef EPD_420bw
GxEPD2_BW<SCREEN, SCREEN::HEIGHT / 2> display(SCREEN(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)
#endif

#ifdef EPD_583bw
GxEPD2_BW<SCREEN, SCREEN::HEIGHT> display(SCREEN(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)
#endif

#ifdef EPD_579bw
GxEPD2_BW<SCREEN, SCREEN::HEIGHT> display(SCREEN(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); 
#endif

const int SCRN_SPI_CHAN = 2; // HSPI
SPIClass hspi(HSPI);
static bool refresh = false;
void display_layout()
{
    display.setRotation(0);
    // status
    display.drawFastHLine(0, 20, display.width(), GxEPD_BLACK);
    display.drawFastVLine(85, 0, 20, GxEPD_BLACK);

    // sensor
    display.drawFastHLine(0, 60, display.width(), GxEPD_BLACK);
    display.drawFastHLine(0, 140, display.width(), GxEPD_BLACK);
    display.drawFastVLine(200, 0, 140, GxEPD_BLACK);

    // map
    display.drawFastVLine(0, 140, 100, GxEPD_BLACK);
    display.drawFastVLine(display.epd2.WIDTH - 1, 140, 100, GxEPD_BLACK);
    display.drawFastHLine(0, 240, display.width(), GxEPD_BLACK);

    display.drawFastHLine(0, display.height() - 20, display.width(), GxEPD_BLACK);
}

void display_time(int secs)
{
    int min = secs / 60;
    int hour = min / 60;
    int m = min % 60;
    int s = secs % 60;
    int h = hour;
    char temp_str[20];

    sprintf(temp_str, "%01d:%02d:%02d\\0", h, m, s);
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
            display.drawLine(xx, p, xx + 1, y, GxEPD_BLACK);
            p = y;
        }
    }
}
int pixels[32][32];
void render(int secs, float hr, float power, float speed, int offset_x)
{
    // clear();
    display.setPartialWindow(0, 0, display.width(), display.height());
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);

    Serial.println("fill white");
    Serial.println(display.width());
    Serial.println(display.height());
    display_layout();
    Serial.println("layout");
    // // // int a = system_bar_h + Font12.Height + Font24.Height;
    // display_status_bar_content(secs);
    display.setCursor(display.width()/2, 100);
    // display.print(String(String(speed, 1) + String("km/h")).c_str());
    display.setCursor(display.width()/2, 50);

    display.setFont(&FreeMonoBold24pt7b);
    display.print(String(hr, 0).c_str());
    display.setCursor(202, 200);
    display.print(String(power, 0).c_str());
    // display.print(String(198).c_str());

    auto xx = 300;
    auto yy = 300;
    display.drawFastHLine(xx, yy, 10, GxEPD_BLACK);
    display.drawFastHLine(xx, yy + 10, 10, GxEPD_BLACK);
    display.drawFastVLine(xx, yy, 10, GxEPD_BLACK);
    display.drawFastVLine(xx + 10, yy, 10, GxEPD_BLACK);

    // display.setFont(&FreeMonoBold9pt7b);
    // // display_chart(hr->queue, 0);
    // // display_chart(power->queue, 201);

    display.displayWindow(0, 0, display.width(), display.height());
    display.display(true);
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
    display.fillScreen(GxEPD_BLACK);
    display.display();
}

void partial_update(String k)
{
    display.setPartialWindow(0, 0, display.width(), 480);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 20);
    display.print(k.c_str());
    display.display(true); // partial update
    Serial.println("partial_update done");
}

void display_init()
{
    hspi.begin(EPD_SCK_PIN, 12, EPD_MOSI_PIN, 15);                                         // remap hspi for EPD (swap pins)
    display.init(115200, true, 2, false, hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0)); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse

    // partial_update(
    //     String("██╗   ██╗███████╗██╗      ██████╗   ") +
    //     String("██║   ██║██╔════╝██║     ██╔═══██╗  ") +
    //     String("██║   ██║█████╗  ██║     ██║   ██║  ") +
    //     String("╚██╗ ██╔╝██╔══╝  ██║     ██║   ██║  ") +
    //     String(" ╚████╔╝ ███████╗███████╗╚██████╔╝  ") +
    //     String("  ╚═══╝  ╚══════╝╚══════╝ ╚═════╝   ") +
    //     String("                                    ") +
    //     String("███╗   ███╗███████╗████████╗ █████╗ ") +
    //     String("████╗ ████║██╔════╝╚══██╔══╝██╔══██╗") +
    //     String("██╔████╔██║█████╗     ██║   ███████║") +
    //     String("██║╚██╔╝██║██╔══╝     ██║   ██╔══██║") +
    //     String("██║ ╚═╝ ██║███████╗   ██║   ██║  ██║") +
    //     String("╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝"));
}

void display_task_code(void *parameter)
{
    Serial.println("display_task_code");
    long last = 0;
refresh_screen();
    std::srand(std::time(nullptr)); // use current time as seed for random generator

    for (;;)
    {
        long secs = millis();
        if (refresh)
        {
            // refresh_screen();
            refresh = false;
        }

        // auto a = current_activity();
        // render(secs / 1000, a->get_hr(1).last, a->get_power(1).last, a->get_speed().last, 2);

        render(secs / 1000, (std::rand() % 100) / 1.0f, 2.f, 2.f, 2);
        Serial.println("render done");
    }
}