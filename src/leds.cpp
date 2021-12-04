#include "leds.hpp"

#include "def.hpp"

void LedStrip::setup() {
  pinMode( LED_STRIP_ENABLE_PIN, OUTPUT );
  enable();
  m_ledControler = &FastLED.addLeds<WS2812B, LED_STRIP_DATA_PIN, GRB>( m_leds, NUM_LEDS ).setCorrection( 0xFFFF80 );
  FastLED.setBrightness( LED_BRIGHTNESS );
}

void LedStrip::loop( uint16_t dt ) {
  if( m_anim ) {
    m_anim->step( dt );
  }
  if( ( m_anim->getState() == AnimState::RUNNING && m_timeSinceLastUpdate + dt >= LED_ANIM_UPDATE_RATE ) ||
      m_timeSinceLastUpdate + dt >= LED_UPDATE_RATE ) {
    FastLED.show();
    m_timeSinceLastUpdate = 0;
  } else {
    m_timeSinceLastUpdate += dt;
  }
}

void LedStrip::enable() {
  digitalWrite( LED_STRIP_ENABLE_PIN, HIGH );
}

void LedStrip::disable() {
  digitalWrite( LED_STRIP_ENABLE_PIN, LOW );
}

LedStrip* LedStrip::setLedColor( uint8_t led, CRGB color ) {
  m_leds[led] = color;
  return this;
}

CRGB LedStrip::getLedColor( uint8_t led ) {
  return m_leds[led];
}

LedStrip* LedStrip::setStripColor( CRGB color ) {
  for( size_t i = 0; i < NUM_LEDS; i++ ) {
    m_leds[i] = color;
  }
  return this;
}

LedStrip* LedStrip::setAnim( StripAnim* anim ) {
  anim->setLeds( m_leds );
  AnimPlayer::setAnim( anim );
  return this;
}
