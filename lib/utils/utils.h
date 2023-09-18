#ifndef UTILS_H_
#define UTILS_H_

#include <Arduino.h>

int boolString2BoolArray(String in, bool *out);

bool char2Bool(char in);

double dBMapping(double in, double inMin, double inMax, double outMin, double outMax);

#endif