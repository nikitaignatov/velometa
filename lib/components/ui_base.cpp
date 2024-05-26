#include "ui_base.hpp"

static const char *TAG = "UI_Base";

static void callback_internal(lv_event_t *e)
{
    ESP_LOGD(TAG, "callback_internal.start");
    auto base = (UI_Base *)lv_event_get_user_data(e);
    ESP_LOGD(TAG, "callback_internal.base");
    if (base == nullptr)
    {
        ESP_LOGD(TAG, "callback_internal.base.nullprt");
        return;
    }
    ESP_LOGD(TAG, "callback_internal.callback");
    base->callback(e);
    ESP_LOGD(TAG, "callback_internal.start");
}

void UI_Base::set_callback(vm_lv_event_cb_t cb, lv_event_code_t code)
{
    ESP_LOGD(TAG, "set_callback.lv_obj_add_event_cb.start");
    lv_obj_add_event_cb(_obj, callback_internal, code, this);
    ESP_LOGD(TAG, "set_callback._cb.assign");
    _cb = cb;
    ESP_LOGD(TAG, "set_callback.done");
}

void UI_Base::callback(lv_event_t *e)
{
    ESP_LOGD(TAG, "callback.start");
    if (_cb == nullptr)
    {
        ESP_LOGD(TAG, "callback._cb.nullprt");
        return;
    }
    ESP_LOGD(TAG, "callback._cb");
    _cb(e);
    ESP_LOGD(TAG, "callback.done");
}

void UI_Base::set_obj(lv_obj_t *obj)
{
    _obj = obj;
}

lv_obj_t *UI_Base::get_obj()
{
    return _obj;
}
