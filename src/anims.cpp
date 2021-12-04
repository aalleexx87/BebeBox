#include "anims.hpp"

void Anim::step( uint16_t dt ) {
  if( m_state == AnimState::WAITING ) {
    if( m_delay > dt ) {
      m_delay -= dt;
    } else {
      m_delay = 0;
      m_state = AnimState::RUNNING;
    }
  } else if( m_state == AnimState::RUNNING ) {
    calcProgress( dt );
    this->internalStep( dt );
  }
}

void Anim::start( uint32_t delay ) {
  m_progressMs = 0;
  if( delay > 0 ) {
    m_delay = delay;
    m_state = AnimState::WAITING;
  } else {
    m_state = AnimState::RUNNING;
  }
};

void Anim::setDuration( uint32_t duration ) {
  stop();
  m_duration = duration;
}

void Anim::setCallback( funcPtr callback ) {
  m_callback = callback;
}

void Anim::stop() {
  m_progressMs = 0;
  m_state = AnimState::FINISHED;
};

AnimState Anim::getState() {
  return m_state;
}

float Anim::calcProgress( uint16_t dt ) {
  m_progressMs += dt;
  if( m_duration > 0 && m_progressMs < m_duration ) {
    m_progress = float( m_progressMs ) / float( m_duration );
  } else {
    m_progress = 1.0;
    m_progressMs = m_duration;
    m_state = AnimState::FINISHED;
    if( m_callback )
      m_callback();
  }
  return m_progress;
};

Anim* AnimPlayer::setAnim( Anim* anim ) {
  clearAnim();
  m_anim = anim;
  return m_anim;
}

void AnimPlayer::clearAnim() {
  if( m_anim ) {
    delete m_anim;
    m_anim = nullptr;
  }
}

Anim* AnimPlayer::getAnim() {
  return m_anim;
}

void AnimPlayer::playAnim( uint32_t delay ) {
  if( m_anim ) {
    m_anim->start( delay );
  }
}

void AnimPlayer::stopAnim() {
  if( m_anim ) {
    m_anim->stop();
  }
}
