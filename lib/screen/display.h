
#include <Arduino.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include "queue.h"

UBYTE *BlackImage;
extern const int screen_height;
extern const int screen_width;

void partial_update(String k);
void display_hr(String k);
void display_last_hr(String k);
void display_last_power(String k);
void display_chart(Queue *queue, int x);
void render(int secs, int hr, int power);
void render();
void clear();
void image_init();
void image_free();
void display_init();