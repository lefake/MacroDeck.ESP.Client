#include "utils.h"

int boolString2BoolArray(String in, bool *out)
{
    int index = 0;

    for (int i = 0; i < in.length(); i += 2)
        out[index++] = char2Bool(in.charAt(i));

    return index;
}

bool char2Bool(char in)
{
    return in == '1';
}

double dBMapping(double in, double inMin, double inMax, double outMin, double outMax)
{
    if (in < inMin)
        in = inMin;

    else if (in > inMax)
        in = inMax;

    double normalized = (in - inMin) / (inMax - inMin);
    double mappedValue = log10(1 + 9 * normalized);
    double dB = mappedValue * (outMax - outMin) + outMin;

    return round(dB * 100.0) / 100.0;
}