#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "anims.hpp"

enum BtnState { RELEASED = 0, PRESSED, LONG_PRESSED, LONG_LONG_PRESSED };

class Button : public AnimPlayer {
 public:
  void setup( uint8_t ledPin, uint8_t btnPin, CRGB color );
  void loop( uint16_t dt );

  Button* setAnim( ButtonAnim* anim );

  CRGB getColor();

  Button* setLedBrightness( uint8_t brightness );
  uint8_t getLedBrightness();

  BtnState getBtnState();
  BtnState getBtnPrevState();
  bool btnStateChanged();
  uint64_t getLastStateChangeTime();

 private:
  uint8_t m_ledPin;
  uint8_t m_btnPin;
  CRGB m_color;

  uint8_t m_ledBrightness = 0;

  BtnState m_btnState = BtnState::RELEASED;
  BtnState m_btnPrevState = BtnState::RELEASED;
  bool m_btnStateChanged = false;

  uint64_t m_lastChangeTime;
};

#endif  // BUTTON_HPP