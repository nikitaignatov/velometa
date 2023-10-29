
#include <Arduino.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include "queue.h"
#include "hr.h"
#include "power.h"
#include "speed.h"

extern const int screen_height;
extern const int screen_width;

void render(int secs, HR *hr, Power *power,Speed *speed);
void clear();
void image_init();
void image_free();
void display_init();
void partial_update(String);