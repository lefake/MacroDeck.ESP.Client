#ifndef SLIDERS_H_
#define SLIDERS_H_

#include <Arduino.h>

#ifndef ROLLING_LENGTH
#define ROLLING_LENGTH  50
#endif

#ifndef CHANGE_THRES    
#define CHANGE_THRES       (0.05)
#endif 

#ifndef MIN_DB
#define MIN_DB          (20.00)
#endif

#ifndef MAX_DB
#define MAX_DB          (36.12)
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
    double todB(double value);
    double round2Digit(double value);
public:
    Sliders();
    ~Sliders();

    void init(const uint8_t pin);
    void update();
    bool getCurrent(double *current);
};

#endif