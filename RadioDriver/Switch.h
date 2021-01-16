#pragma once
#include <Arduino.h>

class Switch {
  public:
  class Receiver {
    public:
    virtual void onPressed() = 0;
    virtual void onReleased() = 0;
  };

private:
  int _pin;
  int _state;
  int _activeState;
  Receiver* _receiver;
  
public:

  Switch(int pin, int activeState = LOW);
  void connect(Receiver* rx);
  void pulse();
  bool active();

  virtual void onPressed();
  virtual void onReleased();
};
