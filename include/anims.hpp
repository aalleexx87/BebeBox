#ifndef ANIMS_HPP
#define ANIMS_HPP

#include "def.hpp"

enum AnimState { WAITING, RUNNING, FINISHED };

typedef void ( *funcPtr )( void );
class Anim {
 public:
  virtual ~Anim(){};

  void step( uint16_t dt );
  void start( uint32_t delay = 0 );
  void setDuration( uint32_t duration );
  void setCallback( funcPtr callback );
  void stop();

  AnimState getState();

 protected:
  Anim( uint32_t duration, funcPtr callback = nullptr, bool loop = false )
      : m_duration( duration ), m_callback( callback ), m_loop( loop ){};
  virtual void internalStep( uint16_t dt ) = 0;
  float calcProgress( uint16_t dt );

  AnimState m_state = AnimState::FINISHED;
  uint32_t m_delay;
  uint32_t m_duration;
  uint32_t m_progressMs = 0;
  funcPtr m_callback;
  float m_progress = 0;
  bool m_loop;
};

class AnimPlayer {
 public:
  AnimPlayer(){};

  Anim* setAnim( Anim* anim );
  void clearAnim();
  Anim* getAnim();
  void playAnim( uint32_t delay = 0 );
  void stopAnim();

 protected:
  Anim* m_anim = nullptr;
};

class Delay : public Anim {
 public:
  Delay( uint32_t duration = 0, funcPtr callback = nullptr ) : Anim( duration, callback ) {}

 protected:
  void internalStep( uint16_t dt ) {}
};

class ButtonAnim : public Anim {
 public:
  ButtonAnim( uint32_t duration = 0, funcPtr callback = nullptr ) : Anim( duration, callback ) {}
  void setLed( uint8_t* led ) { m_led = led; }

 protected:
  uint8_t* m_led;
};

class StripAnim : public Anim {
 public:
  StripAnim( uint32_t duration = 0, funcPtr callback = nullptr ) : Anim( duration, callback ) {}
  void setLeds( CRGB* leds ) { m_leds = leds; }

 protected:
  CRGB* m_leds;
};

class SimpleFadeOutAnim : public ButtonAnim {
 public:
  SimpleFadeOutAnim( uint32_t duration = 0, funcPtr callback = nullptr ) : ButtonAnim( duration, callback ){};

 protected:
  void internalStep( uint16_t dt ) { *m_led = 255 - ( 255 * m_progress ); }
};

class BlinkAnim : public ButtonAnim {
 public:
  BlinkAnim( uint32_t duration = 0, uint8_t count = 1, funcPtr callback = nullptr )
      : ButtonAnim( duration, callback ), m_count( count ){};

 protected:
  void internalStep( uint16_t dt ) {
    float f = 2 * m_progress * m_count - 0.5;
    *m_led = 128 + 143 * sin( f * PI ) + 16 * sin( 3 * f * PI );
  }

 private:
  uint8_t m_count;
};

class BatteryIndicatorAnim : public StripAnim {
 public:
  BatteryIndicatorAnim( uint32_t duration, float percent, funcPtr callback = nullptr )
      : StripAnim( duration, callback ), m_percent( percent ){};

 protected:
  void internalStep( uint16_t dt ) {
    for( size_t i = 0; i < NUM_LEDS; i++ ) {
      if( float( i ) / 8.0 < m_percent ) {
        uint8_t scale =
            map( m_progress, 0.125 * i, 0.125 * i + 0.125,
                 ( float( i + 1 ) / 8.0 > m_percent ) ? 255 * ( m_percent - ( float( i ) * 0.125 ) ) : 255 );
        m_leds[i] = m_ledsColor[i].scale8( CRGB( scale, scale, scale ) );
      } else {
        m_leds[i] = CRGB::Black;
      }
    }
  }

 private:
  uint8_t map( float x, float in_min, float in_max, float out_max ) {
    return min( 255, max( 0, ( x - in_min ) * out_max / ( in_max - in_min ) ) );
  }
  CRGB m_ledsColor[8] = { CRGB( 0xFF0000 ), CRGB( 0xDB2400 ), CRGB( 0xB84700 ), CRGB( 0x906F00 ),
                          CRGB( 0x6E9100 ), CRGB( 0x48B700 ), CRGB( 0x25DA00 ), CRGB( 0x00FF00 ) };
  float m_percent = 0;
};

#endif  // ANIMS_HPP