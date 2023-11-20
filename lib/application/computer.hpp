#ifndef _VELOHUB_COMPUTER_HPP_
#define _VELOHUB_COMPUTER_HPP_
#include "metric.hpp"

class Computer
{
    Metric hr, power, speed, elevation;
    Metric hr_zone, power_zone;

public:
    void initialize();
};

#endif