#pragma once

#include "types.hpp"

class Activity;

class System
{
    bool gps_fix;
    Activity *current_activity;

public:
    bool has_fix();
    Activity *get_current_activity();
};
