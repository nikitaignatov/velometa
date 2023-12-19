#include "vh_dashboard.hpp"

MetricW hrm_w;
MetricW pwr_w;

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

    vh_create_chart(parent);
}