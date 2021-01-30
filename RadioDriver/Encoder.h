#pragma once


// Note this class assumes active low pins.
class Encoder {
  public:
  class Receiver {
    public:
    virtual void onUp() = 0;
    virtual void onDown() = 0;;
  };

  private:
  enum class StateT {
    IDLE,
    D1,
    D2,
    D3,
    U1,
    U2,
    U3
  };
  int _pinA;
  int _pinB;
  StateT _state;
  int _previous;
  Receiver* _receiver;
  
  int _val;
  int read();
public:
  Encoder(int pinA, int pinB);
  void connect(Receiver* rx);
  void pulse();
  virtual void onUp();
  virtual void onDown();
};
