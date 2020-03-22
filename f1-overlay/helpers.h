#ifndef HELPERS_H
#define HELPERS_H

#include <cmath>

int getHueForWheelSlip(int percent)
{
    auto multiplier       = 10;
    auto green            = 120;
    auto color_multiplier = (double)green / 100;

    // logarithmic
    int  scaled =
        100 * (log(1 + (percent * multiplier)) / log(1 + (100 * multiplier)));

    auto hue = (green - (color_multiplier * scaled));

    return hue;
}

#endif  // HELPERS_H
