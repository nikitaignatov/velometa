#include "vl53l5cx.hpp"
#include "lvgl.h"
static lv_obj_t *canvas;

#define CANVAS_PIXEL 8
#define CANVAS_WIDTH CANVAS_PIXEL *CANVAS_PIXEL
#define CANVAS_HEIGHT CANVAS_PIXEL *CANVAS_PIXEL
// sensor is very bad outdoors.
bool DistanceSensor::read_sensor()
{
    if (ready)
    {
        Wire.end();
        Wire.begin(POSITION_SENSOR_SDA, POSITION_SENSOR_SCL, 400000);
        if (myImager.isDataReady() == true)
        {
            if (myImager.getRangingData(&measurementData)) // Read distance data into array
            {
                for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth)
                {
                    for (int x = imageWidth - 1; x >= 0; x--)
                    {
                        auto p = measurementData.distance_mm[x + y];
                        auto c = (int)map(4000-p, 0, 4000, 0, 255);
                        c = c > 255 ? 255 : c;
                        c = c < 0 ? 0 : c;
                        auto yy = (y / imageWidth) * CANVAS_PIXEL;
                        for (size_t i = yy; i < (yy) + imageWidth; i++)
                        {
                            for (size_t j = x * CANVAS_PIXEL; j < x * CANVAS_PIXEL + imageWidth; j++)
                            {
                                lv_canvas_set_px_color(canvas, j, i, lv_color_make(c, 0, c / 2));
                            }
                        }

                        // lv_canvas_set_px_color(canvas, x, y / imageWidth, lv_color_make(c, 0, c / 2));
                        Serial.print("\t");
                        Serial.print(measurementData.distance_mm[x + y]);
                    }
                    Serial.println();
                }
                Serial.println();
            }
            return true;
        }
    }
    return false;
}

void DistanceSensor::init()
{
    Wire.end();
    Wire.begin(POSITION_SENSOR_SDA, POSITION_SENSOR_SCL, 400000);
    if (myImager.begin() == false)
    {
        Serial.println(F("Sensor not found - check your wiring. Freezing"));
        while (1)
            ;
    }
    myImager.setResolution(8 * 8); // Enable all 64 pads
    myImager.setRangingFrequency(4);

    imageResolution = myImager.getResolution(); // Query sensor for current resolution - either 4x4 or 8x8
    imageWidth = sqrt(imageResolution);         // Calculate printing width

    myImager.startRanging();
    ready = true;

    canvas = lv_canvas_create(lv_scr_act());
    /*Create a buffer for the canvas*/
    static lv_color_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

    /*Create a canvas and initialize its palette*/

    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);

    lv_canvas_fill_bg(canvas, lv_color_make(0, 0, 0), LV_OPA_COVER);
}