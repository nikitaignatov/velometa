
#define LV_CONF_INCLUDE_SIMPLE
#define LV_USE_THEME_NIGHT 1
/* Uncomment below line to draw on screen with touch */
#define DRAW_ON_SCREEN

void vh_setup();
void vh_loop();

void update_hr(std::string);
void update_power(std::string);
void update_speed(std::string);