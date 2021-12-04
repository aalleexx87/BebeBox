#include "memory_game.hpp"

CRGB colormap( uint8_t input ) {
  return input == 0 ? CRGB::Red : input == 1 ? CRGB::Green : input == 2 ? CRGB::Blue : CRGB::Yellow;
}

MemoryGame::MemoryGame( LedStrip* ledStrip, Button* buttons )
    : m_ledStrip( ledStrip ), m_buttons( buttons ), m_state( STOPPED ) {}

void MemoryGame::start() {
  for( size_t i = 0; i < NUM_BTN; i++ ) {
    m_buttons[i].setLedBrightness( 0 );
  }

  for( size_t i = 0; i < MAX_LEVEL; i++ ) {
    m_numberList[i] = random8( 4 );
  }
  m_state = GameState::SHOWING_SEQUENCE;
  m_level = 1;
  m_shownStep = 0;
  m_inputStep = 0;
  m_ledStrip->setStripColor( CRGB::Black );
}

void MemoryGame::loop( uint16_t dt ) {
  if( m_state == GameState::SHOWING_SEQUENCE ) {
    if( m_shownStep < m_level ) {
      if( m_shownStep == 0 || m_buttons[m_numberList[m_shownStep - 1]].getAnim()->getState() == AnimState::FINISHED ) {
        m_buttons[m_numberList[m_shownStep]]
            .setAnim( new BlinkAnim( m_level - m_shownStep > 1 ? 1000 - ( analogRead( RED_POT_PIN ) >> 1 ) : 1000, 1 ) )
            ->playAnim();
        m_shownStep++;
      }
    } else {
      m_shownStep = 0;
      m_inputStep = 0;
      m_state = GameState::AWAIT_INPUT;
      m_ledStrip->setStripColor( CRGB::Green );
    }
  }
}

GameState MemoryGame::getState() {
  return m_state;
}

void MemoryGame::unpause() {
  m_state = SHOWING_SEQUENCE;
  m_ledStrip->setStripColor( CRGB::Blue );
}

void MemoryGame::input( uint8_t btn ) {
  if( m_numberList[m_inputStep] == btn ) {
    m_inputStep++;
    if( m_inputStep == m_level ) {
      m_level++;
      m_state = GameState::PAUSE;
    }
  } else {
    m_ledStrip->setStripColor( CRGB::Red );
    m_state = GameState::GAMEOVER;
  }
}

void MemoryGame::gameOver() {}
