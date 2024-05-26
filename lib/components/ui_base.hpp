#pragma once

#include <config.hpp>
#include <types.hpp>
#include <lvgl.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <functional>

class UI_Base
{
public:
    using vm_lv_event_cb_t = std::function<void(lv_event_t *)>;

private:
    vm_lv_event_cb_t _cb;
    lv_obj_t *_obj;

protected:
    UI_Base() {}
    void set_obj(lv_obj_t *obj);

public:
    ~UI_Base()
    {
        lv_obj_del_async(_obj);
        _obj = nullptr;
    }
    lv_obj_t *get_obj();
    void set_callback(vm_lv_event_cb_t cb, lv_event_code_t code);
    void callback(lv_event_t *e);
};