#include <Arduino.h>
#include "Buttons.h"
#include "Sliders.h"
#include "constants.h"

// ===== Macro cb =====

static void muteCallback(const int id);

static void macro1(const int id);
static void macro2(const int id);
static void macro3(const int id);

// ===== Tasks =====

static void pollButtonsTask(void * parameter);
static void pollSlidersTask(void * parameter);
static void sendSlidersTask(void * parameter);

// ===== Buttons variables =====

static Buttons buttons[NB_BUTTONS];
static ButtonCallback cbs[NB_BUTTONS] = { macro1, macro2, macro3 };

// ===== Sliders variables ======

static Sliders sliders[NB_SLIDERS];

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif 

  // Init buttons
  for (int i = 0; i < NB_BUTTONS; ++i)
    buttons[i].init(buttonPins[i], buttonActiveState[i], cbs[i]);

  for (int i = 0; i < NB_SLIDERS; ++i)
    sliders[i].init(sliderPins[i]);

  // Start Tasks
  xTaskCreate(pollButtonsTask, "Poll Buttons", 1000, NULL, 1, NULL);
  xTaskCreate(pollSlidersTask, "Poll Sliders", 5000, NULL, 1, NULL);
  xTaskCreate(sendSlidersTask, "Send Sliders", 5000, NULL, 1, NULL);
}

void loop()
{

}

void pollButtonsTask(void * parameter)
{
  for(;;)
  {
    for (int i = 0; i < NB_BUTTONS; ++i)
      buttons[i].update();

    vTaskDelay(BUTTONS_POLLING_RATE / portTICK_PERIOD_MS);
  }
}

void pollSlidersTask(void * parameter)
{
  for(;;)
  {
    for (int i = 0; i < NB_SLIDERS; ++i)
      sliders[i].update();

    vTaskDelay(SLIDERS_POLLING_RATE / portTICK_PERIOD_MS);
  }
}

void sendSlidersTask(void * parameter)
{
  for(;;)
  {
    bool updated = false;

    for (int i = 0; i < NB_SLIDERS; ++i)
    {
      double c;
      if(sliders[i].getCurrent(&c))
      {
        Serial.printf("%f ", c);
        updated = true;
      }
    }
    
    if (updated)
      Serial.println();

    vTaskDelay(SLIDERS_SENDING_RATE / portTICK_PERIOD_MS);
  }
}

// ========== CB ==========

void muteCallback(const int id)
{
  
}

void macro1(const int id)
{
  Serial.println("Btn1 CB");
}

void macro2(const int id)
{
  Serial.println("Btn2 CB");
}

void macro3(const int id)
{
  Serial.println("Btn3 CB");
}