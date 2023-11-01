#include "display.h"

UBYTE *BlackImage;
int system_bar_h = Font16.Height + 1;
int hr_location_x = 0;
int hr_location_y = system_bar_h;
int power_location_x = screen_width / 2;
int power_location_y = system_bar_h;
int padding = 5;

int cell_width = screen_width / 2 - 2;
int cell_height = screen_height - system_bar_h;

void display_cell_header(String label, String value, int x, int y)
{
    Paint_DrawString_EN(x + padding, y, label.c_str(), &Font12, WHITE, BLACK);
    Paint_DrawString_EN(x + padding, y + Font12.Height, value.c_str(), &Font24, WHITE, BLACK);
}

void display_cell_header_small(String label, String value, int x, int y)
{
    Paint_DrawString_EN(x, y, label.c_str(), &Font8, WHITE, BLACK);
    Paint_DrawString_EN(x, y + (Font8.Height), value.c_str(), &Font12, WHITE, BLACK);
}

void display_chart(Queue *queue, int x)
{
    if (queue->size > 0)
    {
        int min = queue->min;
        int range = queue->max - min;
        range = range == 0 ? 1 : range;
        float_t scaled = (front(queue) - min) / (float_t)range;
        int height = screen_height - (Font12.Height + Font8.Height + Font24.Height + 10);

        int p = screen_height - scaled * height;
        for (size_t i = 0; i < queue->size; i++)
        {
            int s = (queue->front + i) % queue->capacity;
            int c = queue->array[s];

            float_t scaled = ((float_t)c - (float_t)min) / (float_t)range;
            int y = screen_height - scaled * height;

            int xx = i + x;
            Paint_DrawLine(xx, p, xx + 1, y, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
            p = y;
        }
    }
}

void display_hr(HR *sensor)
{
    int x = hr_location_x;
    int y = hr_location_y;
    int header = Font24.Width * 3;
    display_cell_header(String("HR:"), String(sensor->last()), x, y);
    display_cell_header_small(String("min"), String(sensor->min()), x + header, y);
    display_cell_header_small(String("max"), String(sensor->max()), x + header + Font12.Width * 3, y);
    display_cell_header_small(String("avg"), String(sensor->avg()), x + header + Font12.Width * 6, y);
    display_cell_header(String("z"), String(sensor->zone()), x + header + Font12.Width * 9, y);
    display_chart(sensor->queue, x);
}

void display_power(Power *sensor)
{
    int x = power_location_x;
    int y = power_location_y;
    int header = Font24.Width * 4;
    display_cell_header(String("POWER:"), String(sensor->last()), x, y);
    display_cell_header_small(String("max"), String(sensor->max()), x + header, y);
    display_cell_header_small(String("avg"), String(sensor->avg()), x + header + Font12.Width * 5, y);
    display_cell_header(String("z"), String(sensor->zone()), x + header + Font12.Width * 7, y);
    display_chart(sensor->queue, x);
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
    Paint_DrawString_EN(4, 0, temp_str, &Font16, WHITE, BLACK);
}

void render(int secs, HR *hr, Power *power, Speed *speed)
{
    clear();
    int a = system_bar_h + Font12.Height + Font24.Height;
    Paint_DrawLine(0, a - 1, screen_width, a - 1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(0, system_bar_h - 1, screen_width, system_bar_h - 1, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(cell_width, system_bar_h, cell_width, screen_height, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    display_time(secs);
    display_hr(hr);
    display_power(power);

    Paint_DrawString_EN(230, 0, String(speed->last()).c_str(), &Font20, WHITE, BLACK);
    Paint_DrawString_EN(230 + (Font20.Width * 2), 4, String("km/h").c_str(), &Font12, WHITE, BLACK);
    EPD_2IN9_Display(BlackImage);
}

void render()
{
    EPD_2IN9_Display(BlackImage);
}

void clear()
{
    Paint_ClearWindows(0, 0, screen_width, screen_height, WHITE);
}

void image_init()
{
    UWORD Imagesize = ((EPD_2IN9_WIDTH % 8 == 0) ? (EPD_2IN9_WIDTH / 8) : (EPD_2IN9_WIDTH / 8 + 1)) * EPD_2IN9_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        while (1)
            ;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_WIDTH, EPD_2IN9_HEIGHT, 270, WHITE);
    Paint_ClearWindows(0, 0, 290, 15, WHITE);
}

void image_free()
{
    printf("Clear...\r\n");
    EPD_2IN9_Init(EPD_2IN9_FULL);
    EPD_2IN9_Clear();

    printf("Goto Sleep...\r\n");
    EPD_2IN9_Sleep();
    free(BlackImage);
    BlackImage = NULL;
}

void display_init()
{
    printf("EPD_2IN9_test Demo\r\n");
    DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN9_Init(EPD_2IN9_FULL);
    EPD_2IN9_Clear();
    DEV_Delay_ms(500);
    image_init();
    EPD_2IN9_Init(EPD_2IN9_PART);
}

void partial_update(String k)
{
    printf("Partial refresh\r\n");
    EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_ClearWindows(0, 0, screen_width, screen_height, WHITE);
    Paint_DrawLine(0, Font16.Height, screen_width, Font16.Height, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawString_EN(0, 0, k.c_str(), &Font12, WHITE, BLACK);

    EPD_2IN9_Display(BlackImage);
}
