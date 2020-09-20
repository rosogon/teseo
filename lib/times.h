#ifndef times_h
#define times_h

/*
 * This class counts the number of times that isLap() is called
 * until a number of ms have elapsed.
 */
class Times {

public:

  Times();
  void start(unsigned span_ms);

  /* 
   * Return -1 if span_ms have not elapsed since last time;
   * otherwise, return number of times isLap was called and reset counter
   */
  long isLap();

private:
  void reset(unsigned long now);

  unsigned long _until;
  int _span;
  long _n = 0;
};

#endif
