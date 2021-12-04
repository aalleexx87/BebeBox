
#include "button.hpp"
#include "leds.hpp"

#define MAX_LEVEL 64
enum GameState { STOPPED, SHOWING_SEQUENCE, AWAIT_INPUT, PAUSE, GAMEOVER };

class MemoryGame {
 public:
  MemoryGame( LedStrip* ledStrip, Button* buttons );
  void start();
  void loop( uint16_t dt );
  GameState getState();
  void unpause();
  void input( uint8_t btn );
  void gameOver();

 private:
  LedStrip* m_ledStrip;
  Button* m_buttons;

  GameState m_state;

  uint8_t m_numberList[MAX_LEVEL];

  uint8_t m_level;
  uint8_t m_shownStep;
  uint8_t m_inputStep;
};