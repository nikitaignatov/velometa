#pragma once

#include <lvgl.h>
#include "container.hpp"
#include "ui_base.hpp"

class StatusBar : public UI_Base
{
private:
    lv_obj_t *obj;

public:
    StatusBar(const StatusBar &temp_obj) = delete;
    StatusBar &operator=(const StatusBar &temp_obj) = delete;

    ~StatusBar()
    {
        lv_obj_del_async(obj);
        delete obj;
    }
    StatusBar(lv_obj_t *parent, uint16_t width);
};