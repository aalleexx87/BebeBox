#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

#include "button.hpp"
#include "def.hpp"
#include "leds.hpp"
#include "memory_game.hpp"

LedStrip ledStrip;
Button buttons[NUM_BTN];
MemoryGame memGame = MemoryGame( &ledStrip, buttons );

enum Mode { COLOR_MIXER = 0, COLOR_MIXER_HSV, BATTERY_DISPLAY, MEMORY_GAME };
Mode mode = Mode::COLOR_MIXER;

uint64_t lastInputBtnChange = 0;
ISR( PCINT2_vect ) {
  lastInputBtnChange = millis();
  PCIFR = 0;
}

void goToSleep() {
  // turn off leds
  ledStrip.disable();
  for( size_t i = 0; i < NUM_BTN; i++ )
    buttons[i].setLedBrightness( 0 );

  // go to sleep
  set_sleep_mode( SLEEP_MODE_PWR_DOWN );
  cli();
  bitClear( PCIFR, PCIF2 );
  sleep_enable();
  sei();

  // turn off brown-out enable in software
  MCUCR = bit( BODS ) | bit( BODSE );
  MCUCR = bit( BODS );
  sleep_cpu();

  // after wakeup
  ledStrip.enable();
  sleep_disable();
  mode = COLOR_MIXER;
}

void setup() {
  // HW setup
  ledStrip.setup();
  buttons[0].setup( RED_LED_PIN, RED_BTN_PIN, CRGB::Red );
  buttons[1].setup( GREEN_LED_PIN, GREEN_BTN_PIN, CRGB::Green );
  buttons[2].setup( BLUE_LED_PIN, BLUE_BTN_PIN, CRGB::Blue );
  buttons[3].setup( YELLOW_LED_PIN, YELLOW_BTN_PIN, CRGB::Yellow );

  // setup interrupts
  PCMSK2 = 0xF0;
  bitSet( PCICR, PCIE2 );
  bitClear( PCIFR, PCIF2 );

  // disable unused CPU functions
  power_spi_disable();
  power_usart0_disable();

  randomSeed( analogRead( A0 ) * analogRead( A2 ) * analogRead( A2 ) * analogRead( A3 ) * analogRead( A4 ) );
}

uint64_t lastLoopTime = 0;
Delay delayAnim = Delay();

void loop() {
  // calculate time step
  uint64_t time = millis();
  uint16_t dt = time - lastLoopTime;
  lastLoopTime = time;
  delayAnim.step( dt );

  // go to sleep after 5min
  if( time - lastInputBtnChange > 300000 ) {
    goToSleep();
  }

  // calc pot values for color mixers
  uint16_t rPot = 0;
  uint16_t gPot = 0;
  uint16_t bPot = 0;
  if( mode == COLOR_MIXER || mode == COLOR_MIXER_HSV ) {
    for( size_t i = 0; i < 8; i++ ) {
      rPot += analogRead( RED_POT_PIN );
      gPot += analogRead( GREEN_POT_PIN );
      bPot += analogRead( BLUE_POT_PIN );
    }
    rPot = rPot >> 5;
    gPot = gPot >> 5;
    bPot = bPot >> 5;
  }

  Button* button;
  for( size_t i = 0; i < NUM_BTN; i++ ) {
    button = buttons + i;
    button->loop( dt );

    if( button->btnStateChanged() ) {
      BtnState state = button->getBtnState();

      switch( state ) {
        case BtnState::RELEASED:
          if( mode == MEMORY_GAME ) {
            if( memGame.getState() == GameState::AWAIT_INPUT ) {
              if( button->getBtnPrevState() == BtnState::PRESSED ) {
                memGame.input( i );
                if( memGame.getState() == GameState::PAUSE ) {
                  delayAnim = Delay( 1500, [] { memGame.unpause(); } );
                  delayAnim.start();
                }
              }
            }
          } else if( button->getBtnPrevState() == BtnState::PRESSED ) {
            button->setAnim( new SimpleFadeOutAnim( 500 ) )->playAnim();
          }
          break;
        case BtnState::PRESSED:
          if( mode == MEMORY_GAME ) {
            if( memGame.getState() == GameState::AWAIT_INPUT ) {
              button->setAnim( new BlinkAnim( 1000 ) )->playAnim();
            }
          } else {
            button->setLedBrightness( 128 );
          }
          break;
        case BtnState::LONG_PRESSED:
          if( mode != MEMORY_GAME ) {
            button->setLedBrightness( 255 );
          }
          break;
        case BtnState::LONG_LONG_PRESSED:
          mode = ( Mode )i;
          button
              ->setAnim( new BlinkAnim( 500, 3,
                                        [] {
                                          if( mode == MEMORY_GAME ) {
                                            memGame.start();
                                          }
                                        } ) )
              ->playAnim();
          break;
      }
    }
  }

  switch( mode ) {
    case COLOR_MIXER: {
      ledStrip.setStripColor( CRGB( rPot, gPot, bPot ) );
    } break;
    case COLOR_MIXER_HSV: {
      ledStrip.setStripColor( CRGB( CHSV( rPot, gPot, bPot ) ) );
    } break;
    case BATTERY_DISPLAY: {
      if( delayAnim.getState() != AnimState::RUNNING ) {
        ledStrip.setAnim( new BatteryIndicatorAnim( 2000, float( analogRead( BATTERY_PIN ) - 655 ) / 205.0 ) )
            ->playAnim();
        delayAnim = Delay( 4000, [] { mode = COLOR_MIXER; } );
        delayAnim.start();
      }
    } break;
    case MEMORY_GAME:
      memGame.loop( dt );
      break;

    default:
      break;
  }

  ledStrip.loop( dt );
}