#ifndef SLIDERS_H_
#define SLIDERS_H_

#include <Arduino.h>
#include "utils.h"

#ifndef ROLLING_LENGTH
#define ROLLING_LENGTH          50
#endif

#ifndef ANALOG_LOW_THRESOLD
#define ANALOG_LOW_THRESOLD     100
#endif

#ifndef ANALOG_HIGH_THRESOLD
#define ANALOG_HIGH_THRESOLD    4095
#endif

#ifndef CHANGE_THRES    
#define CHANGE_THRES            (0.1)
#endif 

#ifndef VM_DB_MIN
#define VM_DB_MIN               (-60.0)
#endif

#ifndef VM_DB_MAX
#define VM_DB_MAX               (12.0)
#endif

class Sliders
{
private:
    uint8_t pin;

    uint8_t rollingIndex = 0;
    double rollingMean[ROLLING_LENGTH] = { 0 };
    double rollingSum = 0;

    double currentdB;
    double lastdB;
    
    uint16_t readValue();
    double updateRolling(double value);
public:
    Sliders();
    ~Sliders();

    void init(const uint8_t pin);
    void update();
    bool getCurrent(double *current);
};

#endif