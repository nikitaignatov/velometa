#include "vh_screen_activity_hr_stats.hpp"

static lv_obj_t *table;
static void draw_part_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
    if (dsc->part == LV_PART_ITEMS)
    {
        uint32_t row = dsc->id / lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);

        if (row == 0)
        {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
        {
            dsc->label_dsc->align = LV_TEXT_ALIGN_RIGHT;
        }

        if ((row != 0 && row % 2) == 0)
        {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
    }
}

static void new_data_cb(lv_event_t *e)
{
    auto activity = current_activity();
    auto f = "{}";

    /*Fill the first column*/
    lv_table_set_cell_value(table, 0, 0, "HR");
    lv_table_set_cell_value(table, 0, 1, "Min");
    lv_table_set_cell_value(table, 0, 2, "Avg");
    lv_table_set_cell_value(table, 0, 3, "Max");

    lv_table_set_col_width(table, 0, 80);
    lv_table_set_col_width(table, 1, 80);
    lv_table_set_col_width(table, 2, 80);
    lv_table_set_col_width(table, 3, 80);

    lv_table_set_cell_value(table, 1, 0, "5s");
    lv_table_set_cell_value(table, 2, 0, "15s");
    lv_table_set_cell_value(table, 3, 0, "30s");
    lv_table_set_cell_value(table, 4, 0, "60s");
    lv_table_set_cell_value(table, 5, 0, "90s");
    lv_table_set_cell_value(table, 6, 0, "2m");
    lv_table_set_cell_value(table, 7, 0, "5m");
    lv_table_set_cell_value(table, 8, 0, "10m");
    lv_table_set_cell_value(table, 9, 0, "15m");
    lv_table_set_cell_value(table, 9, 0, "FTP");
    lv_table_set_cell_value(table, 9, 0, "30m");

    /*Fill the second column*/
    auto hr = activity->get_hr(0);

    for (size_t i = 0; i < 8; i++)
    {
        hr = activity->get_hr(i);
        lv_table_set_cell_value(table, i + 1, 1, fmt::format(f, hr.get_min()).c_str());
        lv_table_set_cell_value(table, i + 1, 2, fmt::format(f, hr.get_avg()).c_str());
        lv_table_set_cell_value(table, i + 1, 3, fmt::format(f, hr.get_max()).c_str());
    }
}

void vh_create_activity_hr_stats(lv_obj_t *parent)
{
    table = lv_table_create(parent);

    /*Set a smaller height to the table. It'll make it scrollable*/
    lv_obj_set_height(table, 400);
    lv_obj_align(table, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_event_cb(table, new_data_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(MSG_NEW_HR, table, 0);

    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(table, draw_part_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
}