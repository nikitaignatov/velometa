#ifndef DISPLAY_420_H
#define DISPLAY_420_H

#include "hr.hpp"
#include "power.hpp"
#include "speed.hpp"
// #include <GFX.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold24pt7b.h>
// #include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

void render(int secs, HR *hr, Power *power, Speed *speed);

#define BOARD_LILY_WRIST 1
#define BOARD BOARD_LILY_WRIST
#if BOARD == BOARD_LILY_WRIST
#define P_CS 15
#define P_DC 2
#define P_RST 17
#define P_BUSY 16
#define EPD_SCK_PIN 14
#define EPD_MOSI_PIN 13
#else
#define P_CS 5
#define P_DC 17
#define P_RST 16
#define P_BUSY 4

#define EPD_SCK_PIN 18
#define EPD_MOSI_PIN 23
#endif
// #define EPD_CS_PIN 15
// #define EPD_RST_PIN 26
// #define EPD_DC_PIN 27
// #define EPD_BUSY_PIN 25

void partial_update(String k);
void refresh_screen();
void display_init();
void display_bottom(float_t h, float_t s, float_t lat, float_t lon);
void show();

#endif