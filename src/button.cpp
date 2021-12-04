#include "button.hpp"

void Button::setup( uint8_t ledPin, uint8_t btnPin, CRGB color ) {
  m_ledPin = ledPin;
  m_btnPin = btnPin;
  m_color = color;
  pinMode( m_ledPin, OUTPUT );
  pinMode( m_btnPin, INPUT_PULLUP );
  digitalWrite( m_ledPin, LOW );
}

void Button::loop( uint16_t dt ) {
  uint32_t now = millis();
  if( now - m_lastChangeTime < DEBOUNCE_TIME ) return;
  bool btnPressed = digitalRead( m_btnPin ) == LOW;
  if( ( !btnPressed && m_btnState != BtnState::RELEASED ) || ( btnPressed && m_btnState == BtnState::RELEASED ) ) {
    m_btnPrevState = m_btnState;
    m_btnState = btnPressed ? BtnState::PRESSED : BtnState::RELEASED;
    m_btnStateChanged = true;
    m_lastChangeTime = now;
  } else if( btnPressed && m_btnState == BtnState::PRESSED && now - m_lastChangeTime > LONG_PRESS_TIME ) {
    m_btnPrevState = m_btnState;
    m_btnState = BtnState::LONG_PRESSED;
    m_btnStateChanged = true;
    m_lastChangeTime = now;
  } else if( btnPressed && m_btnState == BtnState::LONG_PRESSED &&
             now - m_lastChangeTime > ( LONG_LONG_PRESS_TIME - LONG_PRESS_TIME ) ) {
    m_btnPrevState = m_btnState;
    m_btnState = BtnState::LONG_LONG_PRESSED;
    m_btnStateChanged = true;
    m_lastChangeTime = now;
  }

  if( m_anim ) {
    m_anim->step( dt );
  }
  analogWrite( m_ledPin, m_ledBrightness );
}

Button* Button::setAnim( ButtonAnim* anim ) {
  AnimPlayer::setAnim( anim );
  anim->setLed( &m_ledBrightness );
  return this;
}

CRGB Button::getColor() {
  return m_color;
}

Button* Button::setLedBrightness( uint8_t brightness ) {
  stopAnim();
  analogWrite( m_ledPin, brightness );
  m_ledBrightness = brightness;
  return this;
}

uint8_t Button::getLedBrightness() {
  return m_ledBrightness;
}

BtnState Button::getBtnState() {
  m_btnStateChanged = false;
  return m_btnState;
}

BtnState Button::getBtnPrevState() {
  return m_btnPrevState;
}

bool Button::btnStateChanged() {
  return m_btnStateChanged;
}

uint64_t Button::getLastStateChangeTime() {
  return m_lastChangeTime;
}