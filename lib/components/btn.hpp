#pragma once

#include "ui_base.hpp"

class Btn : public UI_Base
{
private:
    const char *text = "";
    lv_obj_t *label = nullptr;
    lv_obj_t *btn = nullptr;

public:
    Btn(const Btn &temp_obj) = delete;
    Btn &operator=(const Btn &temp_obj) = delete;

    ~Btn()
    {
        lv_obj_del_async(label);
        lv_obj_del_async(btn);
        btn = nullptr;
        label = nullptr;
        text = nullptr;
    }

    void on_press(vm_lv_event_cb_t cb);
    Btn(lv_obj_t *parent, bool next);
    void set_text(const char *txt);
};
