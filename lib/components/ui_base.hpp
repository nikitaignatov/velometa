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
    ~UI_Base()
    {
        lv_obj_del_async(_obj);
        _obj = nullptr;
    }

    lv_obj_t *get_obj()
    {
        return _obj;
    }

    using vm_lv_event_cb_t = std::function<void(lv_event_t *)>;
    void set_callback(vm_lv_event_cb_t cb, lv_event_code_t code)
    {
        ESP_LOGW("UI_Base", "set_callback.lv_obj_add_event_cb");
        lv_obj_add_event_cb(_obj, callback_internal, code, this);
        ESP_LOGW("UI_Base", "set_callback._cb");
        _cb = cb;
        ESP_LOGW("UI_Base", "set_callback.done");
    }

    void callback(lv_event_t *e)
    {
        ESP_LOGW("UI_Base", "callback.start");
        if (_cb == nullptr)
        {
            ESP_LOGW("UI_Base", "callback.nullprt");
            return;
        }
        ESP_LOGW("UI_Base", "callback._cb");
        _cb(e);
        ESP_LOGW("UI_Base", "callback.done");
    }

private:
    vm_lv_event_cb_t _cb;
    lv_obj_t *_obj;

    static void callback_internal(lv_event_t *e)
    {
        ESP_LOGW("UI_Base", "callback_internal.start");
        auto base = (UI_Base *)lv_event_get_user_data(e);
        ESP_LOGW("UI_Base", "callback_internal.base");
        if (base == nullptr)
        {
            ESP_LOGW("UI_Base", "callback_internal.base.nullprt");
            return;
        }
        ESP_LOGW("UI_Base", "callback_internal.callback");
        base->callback(e);
        ESP_LOGW("UI_Base", "callback_internal.start");
    }

protected:
    UI_Base() {}
    void set_obj(lv_obj_t *obj)
    {
        _obj = obj;
        // lv_obj_set_user_data(_obj, this);
    }
};