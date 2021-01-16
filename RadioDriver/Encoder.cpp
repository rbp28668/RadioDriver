#include <Arduino.h>
#include "Encoder.h"

Encoder::Encoder(int pinA, int pinB) :
  _pinA(pinA),
  _pinB(pinB),
  _state(StateT::IDLE), 
  _previous(0),
  _receiver(0),
  _val(0)
  {
  pinMode(_pinA, INPUT_PULLUP);
  pinMode(_pinB, INPUT_PULLUP);
  _previous = read();
}

void Encoder::connect(Receiver* rx) {
  _receiver = rx;
}

int Encoder::read() {
  int status = ((digitalRead(_pinA) == HIGH) ? 2 : 0) + ((digitalRead(_pinB) == HIGH) ? 1 : 0);
  return status;
}

void Encoder::pulse() {
  
  int val = read();
  if(val != _previous) { // encoder changed?
    
    //Serial.print((int)_state);
    //Serial.print(":");
    //Serial.print(val);
    //Serial.println();
    
    switch(_state) {
      case StateT::IDLE:
        if(val == 2) _state = StateT::D1;
        if(val == 1) _state = StateT::U1;
      break;
      
      case StateT::D1:
        if(val == 0) _state = StateT::D2;
        if(val == 3) _state = StateT::IDLE;
      break;
  
      case StateT::D2:
        if(val == 1) _state = StateT::D3;
        if(val == 2) _state = StateT::D1;
      break;
  
      case StateT::D3:
        if(val == 3) {
          onDown();
          _state = StateT::IDLE;
        }
        if(val == 0) _state = StateT::D2;
      break;
  
      case StateT::U1:
        if(val == 0) _state = StateT::U2;
        if(val == 3) _state = StateT::IDLE;
      break;
  
      case StateT::U2:
        if(val == 2) _state = StateT::U3;
        if(val == 1) _state = StateT::U1;
      break;
  
      case StateT::U3:
        if(val == 3) {
          onUp();
          _state = StateT::IDLE;
        }
        if(val == 0) _state = StateT::U2;
      break;
    }
    _previous = val; 
  }
  
}

void Encoder::onUp() {
  if(_receiver) {
    _receiver->onUp();
  }
}
void Encoder::onDown() {
  _receiver->onDown();
}
