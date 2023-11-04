#ifndef DISPLAY_420_H
#define DISPLAY_420_H

#include "hr.h"
#include "power.h"
#include "speed.h"
// #include <GFX.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold24pt7b.h>
// #include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>


void render(int secs, HR *hr, Power *power, Speed *speed,int screen);
void display_map_dark(int16_t offset_x,int16_t offset_y);

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

void partial_update(String k);
void refresh_screen();
void display_init();

#endif