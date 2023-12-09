#include "vh_mock_btn.hpp"

void mock_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        vh_mock_data_toggle();
        LV_LOG_USER("mock_event_handler::clicked");
    }
}

lv_obj_t *vh_create_mock_btn(lv_obj_t *parent)
{
    lv_obj_t *btn_m = lv_btn_create(parent);
    lv_obj_add_event_cb(btn_m, mock_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_flag(btn_m, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(btn_m, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(btn_m, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(btn_m, lv_color_hex(0x090909), LV_PART_MAIN);

    lv_obj_t *label_m = lv_label_create(btn_m);
    lv_label_set_text(label_m, "Mock Data");
    lv_obj_set_style_bg_opa(label_m, 0, LV_PART_MAIN);

    return btn_m;
}