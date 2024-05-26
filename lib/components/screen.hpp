#pragma once

#include <lvgl.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "vh_gps.hpp"
#include "vh_screen_brightness.hpp"
#include "vh_dashboard.hpp"
#include "status_bar.hpp"
#include "screen_cda.hpp"
#include "ui_base.hpp"
#include "btn.hpp"

class Tile
{
};

class Display
{
private:
    uint8_t active = 0;
    uint8_t pages = 0;
    lv_obj_t *_parent = nullptr;
    lv_obj_t *tv = nullptr;
    uint16_t width = 320, height = 450;
    Btn *btn_next = nullptr, *btn_prev = nullptr;
    StatusBar *status = nullptr;

    void next_tile();
    void prev_tile();

public:
    Display(const Display &temp_obj) = delete;
    Display &operator=(const Display &temp_obj) = delete;

    ~Display()
    {
        lv_obj_del_async(tv);
        delete tv;

        delete status;
        delete btn_prev;
        delete btn_next;
        delete _parent;
    }

    Display(lv_obj_t *parent, uint16_t w, uint16_t h);
    void set_width(uint16_t w);
    void set_height(uint16_t h);
};