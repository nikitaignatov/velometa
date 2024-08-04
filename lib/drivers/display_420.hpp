#pragma once

#include <string>
#include <cstdlib>
#include <ctime>

// #include "activity.hpp"
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMono24pt7b.h>
// #include <Fonts/FreeMono18pt7b.h>
// #include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include "config.hpp"
#include "queue.hpp"

// #define EPD_213bw
// #define EPD_75bwr
// #define EPD_75bw
// #define EPD_420bw
#define EPD_583bw


#ifdef EPD_213bw
#include <GxEPD2_BW.h>
#define SCREEN GxEPD2_213_B74
extern GxEPD2_BW<SCREEN, SCREEN::HEIGHT> display;
#endif
#ifdef EPD_75bwr
#include <GxEPD2_3C.h>
#define SCREEN GxEPD2_750c
extern GxEPD2_3C<SCREEN, SCREEN::HEIGHT / 24> display;
#endif

#ifdef EPD_420bw
#include <GxEPD2_BW.h>
#define SCREEN GxEPD2_420
extern GxEPD2_BW<SCREEN, SCREEN::HEIGHT / 2> display;
#endif
#ifdef EPD_583bw
#include <GxEPD2_BW.h>
#define SCREEN GxEPD2_583_T8
extern GxEPD2_BW<SCREEN, SCREEN::HEIGHT > display;
#endif


void partial_update(std::string k);
void refresh_screen();
void display_init();
void display_task_code(void *parameter);
