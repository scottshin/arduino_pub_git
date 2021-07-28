/*

Demonstrates the use of the Parallax WS2812B RGB Led Module (#28085)

This demo cycles two LEDs through the three primary colors, red, blue, and
green, slowly fading in for each color.

For a demonstration using just one WS2812B RGB LED module, please see the 
'WS2812B_Single' sketch.

------------

This sketch REQUIRES the use of a third-party library, Adafruit_NeoPixel. 
Refer to the 'WS2812B_Single' sketch for important setup information
for this library.

This example code is for the Arduino Uno and direct compatible boards, using the
Arduino 1.0 or later IDE software. It has not been tested, nor designed for, other 
Arduino boards, including the Arduino Due.

------------

Connections:
First LED    Arduino
GND          GND
5V           5V
SI           Digital Pin 6

First LED    Second LED
GND          GND
5V           5V
SO           SI

Note: Both the 5V and GND connections on the LED modules are thru-connected.
You may connect ground and power to either side of the module.

*/

#include <Adafruit_NeoPixel.h>         // Include Adafruit NeoPixel library
#define PIN          4               // First LED on digital pin 6
#define NUMLEDS        36              // Use total of 2 LEDs

// Create 'leds' object to drive LEDs
//Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);



void setup() {
  strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  strip.show(); 


        uint32_t c = strip.Color(100,250,150);

      for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
    
      }


      strip.show();





 c = strip.Color(255,0,0); 

 strip.setPixelColor(19, c);
           strip.setPixelColor(20, c);

           strip.setPixelColor(22, c);
           strip.setPixelColor(23, c);
           strip.setPixelColor(24, c);
           strip.setPixelColor(25, c );
           strip.setPixelColor(26, c );
             strip.show();  strip.show();

c = strip.Color(0,255, 0);
           strip.setPixelColor(27, c );

                      strip.setPixelColor(28, c );

                      strip.setPixelColor(29, c );
                      
                      strip.setPixelColor(30, c );
   
                      strip.setPixelColor(31, c );
                         
                      strip.setPixelColor(32, c );
                         
                      strip.setPixelColor(33, c );
                         
                      strip.setPixelColor(34, c );
                         
                      strip.setPixelColor(35, c );
                         
                      strip.setPixelColor(36, c );
             strip.show();  strip.show();


          // 20 is 2floor first
}


void loop() {

   // colorWipe(strip.Color(50, 80, 50), 3000);



      
/*
  //아래의 순서대로 NeoPixel을 반복한다.
  colorWipe(strip.Color(255, 0, 0), 50); //빨간색 출력
  colorWipe(strip.Color(0, 255, 0), 50); //녹색 출력
  colorWipe(strip.Color(0, 0, 255), 50); //파란색 출력

  theaterChase(strip.Color(127, 127, 127), 50); //흰색 출력
*/
/*  
  theaterChase(strip.Color(127,   0,   0), 50); //빨간색 출력
  theaterChase(strip.Color(  0,   0, 127), 50); //파란색 출력

  //화려하게 다양한 색 출력
  rainbow(20);
  */
  //rainbowCycle(20);
  
  //theaterChaseRainbow(50);
  

}

//NeoPixel에 달린 LED를 각각 주어진 인자값 색으로 채워나가는 함수
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

//모든 LED를 출력가능한 모든색으로 한번씩 보여주는 동작을 한번하는 함수
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//NeoPixel에 달린 LED를 각각 다른색으로 시작하여 다양한색으로 5번 반복한다
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { 
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//입력한 색으로 LED를 깜빡거리며 표현한다
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//LED를 다양한색으로 표현하며 깜빡거린다
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     //256가지의 색을 표현
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0); 
        }
    }
  }
}

//255가지의 색을 나타내는 함수
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
