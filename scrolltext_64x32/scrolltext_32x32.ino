// scrolltext demo for Adafruit RGBmatrixPanel library.
// Demonstrates double-buffered animation on our 16x32 RGB LED matrix:
// http://www.adafruit.com/products/420

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library


//#include <SoftwareSerial.h>
//SoftwareSerial BTSerial(2, 3); //Connect HC-06. Use your (TX, RX) settings


 

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)


#define LAT   10  // for 32
//#define LAT     (A3) // for 16      

#define OE  9

#define A   A0
#define B   A1
#define C   A2
#define D   A3


#define WW  (32)
// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix( A, B, C, CLK, LAT, OE, true, WW );
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true, 64);

// Double-buffered mode consumes nearly all the RAM available on the
// Arduino Uno -- only a handful of free bytes remain.  Even the
// following string needs to go in PROGMEM:

const char str[] PROGMEM = "QUARTERNOETE 64x32 RGB LED Matrix With BLE module";
int    textX   =  WW/* matrix.width()*/,
       textMin = sizeof(str) * -12,
       hue     = 0;
int8_t ball[3][4] = {
  {  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { 17, 15,  1, -1 },
  { 27,  4, -1,  1 }
};
static const uint16_t PROGMEM ballcolor[3] = {
  0x0080, // Green=1
  0x0002, // Blue=1
  0x1000  // Red=1
};

void setup() {
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(2);
}

void loop() {
  byte i;

  // Clear background
  matrix.fillScreen( 0);
/*
  // Bounce three balls around
  for(i=0; i<3; i++) {
    // Draw 'ball'
    matrix.fillCircle(ball[i][0], ball[i][1], 5, pgm_read_word(&ballcolor[i]));
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];
    // Bounce off edges
    if((ball[i][0] == 0) || (ball[i][0] == (matrix.width() - 1)))
      ball[i][2] *= -1;
    if((ball[i][1] == 0) || (ball[i][1] == (matrix.height() - 1)))
      ball[i][3] *= -1;
  }
*/
  // Draw big scrolly text on top
//  matrix.setTextColor( matrix.Color888(255,255,255, false) );
    matrix.setTextColor( matrix.ColorHSV(hue, 255, 255, true));
  
  
  matrix.setCursor(textX, 1);
  matrix.print(F2(str));


  // Move text left (w/wrap), increase hue
  if((--textX) < textMin) textX = matrix.width();
  hue += 7;
  if(hue >= 2536) hue -= 1536;

  // Update display
  matrix.swapBuffers(false);
}
