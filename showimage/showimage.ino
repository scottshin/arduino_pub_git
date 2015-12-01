// colorwheel_progmem demo for Adafruit RGBmatrixPanel library.
// Renders a nice circle of hues on our 32x32 RGB LED matrix:
// http://www.adafruit.com/products/607

// This version uses precomputed image data stored in PROGMEM
// rather than calculating each pixel.  Nearly instantaneous!  Woo!

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include "image.h"


#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 32);

void setup() {
  int     i, len;
  uint8_t *ptr = matrix.backBuffer(); // Get address of matrix data

  
  // Copy image from PROGMEM to matrix buffer:
   memcpy_P(ptr, img,30);//sizeof(img));

  // Start up matrix AFTER data is copied.  The RGBmatrixPanel
  // interrupt code ties up about 40% ofk the CPU time, so starting
  // it now allows the prior drawing code to run even faster!
  matrix.begin();

  
/*
//for ( int c = 0; c < 0xff; c++)
{
 // uint16_t  cr = matrix.Color888(0,0,0);
 
  
  for ( int x = 0; x <10; x++)
    for ( int y = 0; y < 10; y++)
         matrix.drawPixel(x,y,  matrix.ColorHSV(0,100,100,true));

}
*/
}

void loop() {
  // do nothing
}
