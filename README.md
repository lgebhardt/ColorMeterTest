  This is a proof of concept RGB light meter intended for darkroom enlarger
  readings.

  Designed to work with:
  
  An Adafruit AS7262 breakout:
  ----> http://www.adafruit.com/products/3779

  A Seed Studio XIAO 
  ----> https://www.seeedstudio.com/xiao-series-page

  And a small OLED display like
  ----> https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/ref=sr_1_2


  These sensors use I2C to communicate. The addresses used by your device may be
  different from what is in this sketch.

  The wiring is very simple with all devide communication taking place over I2C.
