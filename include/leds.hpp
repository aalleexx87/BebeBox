#ifndef LED_STRIP_HPP
#define LED_STRIP_HPP

#include "anims.hpp"

class LedStrip : public AnimPlayer {
 public:
  void setup();
  void loop( uint16_t dt );
  void enable();
  void disable();

  LedStrip* setLedColor( uint8_t led, CRGB color );
  CRGB getLedColor( uint8_t led );
  LedStrip* setStripColor( CRGB color );

  LedStrip* setAnim( StripAnim* anim );

 private:
  uint16_t m_timeSinceLastUpdate;
  CRGB m_leds[NUM_LEDS];
  CLEDController* m_ledControler;
};

#endif  // LED_STRIP_HPP