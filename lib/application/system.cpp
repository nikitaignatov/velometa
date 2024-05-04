#include "system.hpp"

System sys;

Activity *System::get_current_activity()
{
    return current_activity;
}

bool System::has_fix()
{
    return gps_fix;
}
