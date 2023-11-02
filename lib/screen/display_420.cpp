#include "display_420.h"

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(GxEPD2_420_M01(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)
const int SCRN_SPI_CHAN = 2;                                                                                                           // HSPI
SPIClass hspi(HSPI);

void display_layout()
{
    display.drawFastHLine(0, 20, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
    display.drawFastHLine(0, 60, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
    display.drawFastHLine(0, 140, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
    display.drawFastVLine(200, 0, 140, GxEPD_BLACK);
    display.drawFastHLine(0, GxEPD2_420_M01::HEIGHT - 20, GxEPD2_420_M01::WIDTH, GxEPD_BLACK);
}

void display_status_bar_content(int secs)
{
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 10);
    display.print(String(secs).c_str());
}

void render(int secs, HR *hr, Power *power, Speed *speed)
{
    // clear();
    // int a = system_bar_h + Font12.Height + Font24.Height;
    display.fillScreen(GxEPD_WHITE);
    display_layout();
    display_status_bar_content(secs);
    display.setCursor(0, 55);
    display.setFont(&FreeMonoBold24pt7b);
    // display.print(String(hr->last()).c_str());
    display.print(String(198).c_str());

    display.setFont(&FreeMonoBold9pt7b);
    display.display(true);
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