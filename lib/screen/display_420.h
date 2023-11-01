#ifndef DISPLAY_H
#define DISPLAY_H

#include "hr.h"
#include "power.h"
#include "speed.h"
// #include <GFX.h>
#include <GxEPD2_BW.h>
// #include <Fonts/FreeMonoBold24pt7b.h>
// #include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

extern const int screen_height;
extern const int screen_width;

void render(int secs, HR *hr, Power *power, Speed *speed);

/**
 *
 * Pin	ESP32	  Description
 * VCC	3V3	    Power input (3.3V)
 * GND	GND	    Ground
 * DIN	P14	    SPI MOSI pin, data input
 * SCLK	P13	    SPI CLK pin, clock signal input
 * CS	  P15	    Chip selection, low active
 * DC	  P27	    Data/command, low for commands, high for data
 * RST	P26	    Reset, low active
 * BUSY	P25	    Busy status output pin (means busy)
 */

#define P_CS 5
#define P_DC 17
#define P_RST 16
#define P_BUSY 4

#define EPD_SCK_PIN 18
#define EPD_MOSI_PIN 23
// #define EPD_CS_PIN 15
// #define EPD_RST_PIN 26
// #define EPD_DC_PIN 27
// #define EPD_BUSY_PIN 25

GxEPD2_BW<GxEPD2_420_M01, GxEPD2_420_M01::HEIGHT> display(GxEPD2_420_M01(/*CS=5*/ P_CS, /*DC=*/P_DC, /*RST=*/P_RST, /*BUSY=*/P_BUSY)); // GDEW042M01 400x300, UC8176 (IL0398)
const int SCRN_SPI_CHAN = 2;                                                                                                           // HSPI
SPIClass hspi(HSPI);

void partial_update(String k)
{
    display.setFullWindow();
    // display.setRotation(1);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    // center HelloWorld
    // display.getTextBounds(k.c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
    // // uint16_t hwx = ((display.width() - tbw) / 2) - tbx;
    // // uint16_t hwy = ((display.height() - tbh) / 2) - tby;
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

#endif