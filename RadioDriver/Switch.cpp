#include "Switch.h"

Switch::Switch(int pin, int activeState):
  _pin(pin),
  _activeState(activeState),
  _receiver(0)
{
  pinMode(_pin, INPUT_PULLUP);
  _state = digitalRead(_pin);
}

void Switch::connect(Receiver* rx) {
  _receiver = rx;
}

void Switch::pulse() {
  int val = digitalRead(_pin);
  if(val != _state) {
    if(val == _activeState) {
      onPressed();
    } else {
      onReleased();
    }
    _state = val;
  }
}

bool Switch::active(){
  return _state == _activeState;
}

void Switch::onPressed() {
  if(_receiver) {
    _receiver->onPressed();
  }
}

void Switch::onReleased() {
  if(_receiver) {
    _receiver->onReleased();
  }
}
