#include "vh_dashboard.hpp"

MetricW hrm_w;
MetricW pwr_w;
lv_obj_t *label;

static void histogram_update_event_cb(lv_event_t *e)
{
    auto ac = current_activity();
    lv_label_set_text_fmt(label, "load: %.2f w power: %u intensity: %u%", ac->get_tl(), ac->get_xpower(),ac->get_intensity());
}

void vh_create_dashboard(lv_obj_t *parent)
{
    hrm_w.init(parent);
    hrm_w.set_zone_converter(zone_from_hr);
    hrm_w.subscribe(MSG_NEW_HR);
    hrm_w.set_label("HR");
    hrm_w.set_opposite(false);
    lv_obj_align(hrm_w.get_object(), LV_ALIGN_TOP_RIGHT, 0, 0);

    pwr_w.init(parent);
    pwr_w.set_zone_converter(zone_from_power);
    pwr_w.subscribe(MSG_NEW_POWER);
    pwr_w.set_label("POWER");
    pwr_w.set_opposite(true);
    lv_obj_align(pwr_w.get_object(), LV_ALIGN_TOP_LEFT, 0, 0);

    vh_create_chart(parent, 320, 150, 120);

    label = lv_label_create(parent);
    lv_label_set_text(label, "load");
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -120);

    lv_obj_add_event_cb(label, histogram_update_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(MSG_NEW_HR, label, 0);
}