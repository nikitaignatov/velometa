#include "display.h"

char temp_str[20];

void partial_update(String k)
{
    printf("Partial refresh\r\n");
    EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_ClearWindows(0, 0, screen_width, screen_height, WHITE);
    Paint_DrawLine(0, 26, screen_width, 26, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawString_EN(0, 0, k.c_str(), &Font20, WHITE, BLACK);

    EPD_2IN9_Display(BlackImage);
}

void display_hr(String k)
{
    printf("display_hr\n");
    EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_ClearWindows(0, 27, screen_width / 2, screen_height / 2, WHITE);
    Paint_DrawString_EN(10, 27, String("HR ").c_str(), &Font12, WHITE, BLACK);
    Paint_DrawString_EN(10, 27 + 12, k.c_str(), &Font24, WHITE, BLACK);
    EPD_2IN9_Display(BlackImage);
}

void display_last_hr(String k)
{
    printf("display_hr\n");
    // EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_ClearWindows(0, 27, screen_width / 2, screen_height / 2, WHITE);
    Paint_DrawString_EN(10, 27, String("HR ").c_str(), &Font12, WHITE, BLACK);
    Paint_DrawString_EN(10, 27 + 12, k.c_str(), &Font24, WHITE, BLACK);
}

void display_last_power(String k)
{
    printf("display_power\n");
    // EPD_2IN9_Init(EPD_2IN9_PART);
    Paint_ClearWindows(screen_width / 2, 27, screen_width, screen_height / 2, WHITE);
    Paint_DrawString_EN(11 + (screen_width / 2), 27, String("POWER ").c_str(), &Font12, WHITE, BLACK);
    Paint_DrawString_EN(11 + (screen_width / 2), 27 + 12, k.c_str(), &Font24, WHITE, BLACK);
}

void display_chart(Queue *queue, int x)
{
    if (queue->size > 0)
    {
        int p = screen_height - front(queue) / 4;
        for (size_t i = 0; i < queue->size; i++)
        {
            int s = (queue->front + i) % queue->capacity;
            int c = queue->array[s];
            c = screen_height - c / 4;
            int xx = i + x;
            Paint_DrawLine(xx, p, xx + 1, c, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
            p = c;
        }
    }
}

void render(int secs, int hr, int power)
{
    int min = secs / 60;
    int hour = min / 60;
    int m = min % 60;
    int s = secs % 60;
    int h = hour;

    sprintf(temp_str, "%02d:%02d:%02d\0", h, m, s);
    Paint_DrawLine(0, 26, screen_width, 26, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawString_EN(10, 0, temp_str, &Font20, WHITE, BLACK);

    display_last_hr(String(hr));
    display_last_power(String(power));
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
