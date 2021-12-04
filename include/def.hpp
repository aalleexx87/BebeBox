#include <Arduino.h>
#include <FastLED.h>
#include <math.h>

#define BATTERY_PIN A0
#define RED_POT_PIN A1
#define GREEN_POT_PIN A2
#define BLUE_POT_PIN A3

#define LED_STRIP_ENABLE_PIN A4
#define LED_STRIP_DATA_PIN A5

#define RED_BTN_PIN 4
#define GREEN_BTN_PIN 5
#define BLUE_BTN_PIN 6
#define YELLOW_BTN_PIN 7

#define RED_LED_PIN 3
#define GREEN_LED_PIN 9
#define BLUE_LED_PIN 10
#define YELLOW_LED_PIN 11

#define NUM_LEDS 8
#define NUM_BTN 4

#define LONG_PRESS_TIME 500
#define LONG_LONG_PRESS_TIME 3000

#define LED_BRIGHTNESS 128

#define LED_UPDATE_RATE 30
#define LED_ANIM_UPDATE_RATE 10

#define DEBOUNCE_TIME 30