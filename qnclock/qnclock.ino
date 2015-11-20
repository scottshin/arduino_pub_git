// scrolltext demo for Adafruit RGBmatrixPanel library.
// Demonstrates double-buffered animation on our 16x32 RGB LED matrix:
// http://www.adafruit.com/products/420

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library


#include <SPI.h>
#include <SD.h>

#include <SoftwareSerial.h>


#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>





#define HC06 Serial3


File myFile;


// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr


#define SD_CS    (53)    // 7

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT (10)
#define OE  9


#define A   A0
#define B   A1
#define C   A2
#define D   A3






// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);
RGBmatrixPanel matrix( A, B, C, D, CLK, LAT, OE, true, 32);

// Double-buffered mode consumes nearly all the RAM available on the
// Arduino Uno -- only a handful of free bytes remain.  Even the
// following string needs to go in PROGMEM:

const char str[] PROGMEM = "QUARTERNOETE 64x32 RGB LED Matrix With BLE module";
int    textX   = matrix.width(),
       textMin = sizeof(str) * -12,
       hue     = 0;
int8_t ball[3][4] = {
  {  3,  0,  1,  1 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { 17, 15,  1, -1 },
  { 27,  4, -1,  1 }
};



int8_t hour_xy[12][4] = {
  {3,1, 3,2 },  // 12 
  {4,1, -1, -1 },  // 1
  {3,2, -1, -1 },  // 2
  {2,2, -1, -1 },  // 3
  {1,2. -1. -1 },  // 4
  {1,0, 1, 1},  // 5
  {0,1, 1, 1},  // 6
  {2,0, 2, 1},  // 7
  {0,1, 0, 2},  // 8
  {3,0, 4, 0},  // 9
  {3,1, -1, -1},  // 10
  {3,1, 4, 1 }  // 11                 
};

int8_t min_ten[12][6] =
{
  {-1,-1, -1. -1, -1, -1},   // 0
  { 3, 4, -1, -1, -1, -1},   // 5
  { 4, 3, -1, -1, -1, -1},   // 10
  { 2, 4,  3,  4, -1, -1},   // 15
  { 2, 3,  2,  4, -1, -1},   // 20
  { 2, 3,  2,  4,  3, 4 },   // 25
  { 3, 3,  4,  3, -1, -1},   // 30
  { 3, 3,  4,  3,  3, 4},    // 35
  { 0, 4,  2,  4, -1, -1},   // 40
  { 0, 4,  2,  4,  3, 4},    // 45
  { 1, 4,  2,  4, -1, -1},   // 50
  { 1, 4,  2, 4,  3, 4 }     // 55 
};


int8_t min_bun[10][2] = {
    {-1,-1},   // 0
    {-1,-1},   // 0
    {-1,-1},   // 0
    {-1,-1},   // 0
    {-1,-1},   // 4
          
    { 3, 4},   // 5
    { 3, 4},   // 6
    { 3, 4},   // 7
    { 3, 4},   // 8
    { 3, 4},   // 9
};


#define WHITE     3
#define GRAY      4
#define GREEN     0
#define BLUE      1
#define RED       2

static const uint16_t PROGMEM ballcolor[5] = {
  0x0080, // Green=1
  0x0002,  // Blue=1
  0x1000,  // Red=1
   0x1082,  // white
   0x1052 
};

struct PIXEL
{
  int x;
  int y;
};

PIXEL pxHour;
PIXEL pxMin;



void setup() {

  pxHour.x = 0;
  pxHour.y = 0;
  pxMin.x = 0;
  pxMin.y = 0;

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("program start!");
  
  HC06.begin(9600); // set the data rate for the BT port


  Serial.println("bluetooth send on!");
  HC06.write("on!!");
  HC06.write("on!!");
  HC06.write("on!!");
  HC06.write("df n!!\n");
  


  Serial.println("Initializing SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
  }
  Serial.println("initialization done.");
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }



  //
  //
  //
  
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(2);
}

//#define PITCH(X)  ( X*6 + 3) 
int PITCH(int x )
{

  if ( x == 0 )
    return  3+ 1 -1;
  if ( x == 1 )
    return  3 + 6 ;
  if ( x == 2 )
    return  3+ 12 + 1;
  if ( x== 3 )
    return 3 + 18 + 1;
    
  if ( x == 4 )
    return (3 + 24 + 1); 
}

void calc( int x, int y, int &dx, int &dy )
{

  if ( x != dx )
  {
      if ( x > dx)
          dx++;
      else
          dx--; 
  }
  

  if ( y != dy )
  {
      if ( y > dy )
          dy++;
      else
          dy--; 
  }
  
  
}


void display_hour( int hour )
{

    int x = PITCH(hour_xy[hour][0]);
    int y = PITCH(hour_xy[hour][1]);


    calc( x, y, pxHour.x, pxHour.y );
    matrix.fillCircle( pxHour.x, pxHour.y,   2, pgm_read_word(&ballcolor[WHITE]));

      
      if ( hour_xy[hour][2] != -1 )
      {
          matrix.fillCircle( PITCH(hour_xy[hour][2]), PITCH(hour_xy[hour][3]),   2, pgm_read_word(&ballcolor[WHITE]));
      }

      // SI
      matrix.fillCircle( 28, 16,   2, pgm_read_word(&ballcolor[GRAY]));


}

void display_min( int _min )
{

     int min = (_min /5);  

  



     if ( min_ten[min][0] != -1)
     {
          
         int x = PITCH(min_ten[min][0]);
         int y = PITCH(min_ten[min][1]);

          calc( x, y, pxMin.x, pxMin.y );
          matrix.fillCircle( pxMin.x, pxMin.y,   2, pgm_read_word(&ballcolor[WHITE]));


     }
     
     if ( min_ten[min][2] != -1 )
     {
          matrix.fillCircle( PITCH(min_ten[min][2]), PITCH(min_ten[min][3]),   2, pgm_read_word(&ballcolor[WHITE]));
     }

      if ( min_ten[min][4] != -1 )
     {
          matrix.fillCircle( PITCH(min_ten[min][4]), PITCH(min_ten[min][5]),   2, pgm_read_word(&ballcolor[WHITE]));
     }


    if ( min != 0 )
    {   
    // bun
      matrix.fillCircle( 28, 29,   2, pgm_read_word(&ballcolor[GRAY]));
    } 

  
}

bool IsDay( int hour )
{
  if ( hour >= 6 && 19 > hour )
      return true;
  return false;
}

void display_day( int hour )
{
   matrix.fillCircle( PITCH(0), PITCH( IsDay(hour) ? 0 : 3),   2, pgm_read_word(&ballcolor[GREEN]));
}


void loop() {
  byte i;

  // Clear background
  matrix.fillScreen(0);



  tmElements_t tm;
  
  if ( RTC.read(tm) )
  {
      int h = tm.Hour;
      if ( tm.Hour > 12 )
        h = tm.Hour -12;
      display_day( tm.Hour );
      display_hour( h );
      display_min(  tm.Minute );
  }
  else
  {
      tm.Hour = 22; tm.Minute = 10;
      RTC.write(tm);
  }

 
/*
  // Bounce three balls around
  for(i=0; i<3; i++) {
    // Draw 'ball'
    matrix.fillCircle(ball[i][0], ball[i][1], 2, pgm_read_word(&ballcolor[i]));
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


/*
  // Draw big scrolly text on top
  matrix.setTextColor(matrix.ColorHSV(hue, 255, 255, true));
  matrix.setCursor(textX, 8);
  matrix.print(F2(str));
*/  

  // Move text left (w/wrap), increase hue
  if((--textX) < textMin) 
      textX = matrix.width();
  hue += 7;
  if(hue >= 1536) hue -= 1536;



    // Update display
    matrix.swapBuffers(false);


    char data;
    while(HC06.available()) {
      data = HC06.read();
      if(data == -1) break;
      Serial.print(data);
      delay(1);
   }  
/*
  // BT –> Data –> Serial
  if (HC06.available()) {
    Serial.write(HC06.read());
  }
*/
  // Serial –> Data –> BT
  if (Serial.available()) {
    HC06.write(Serial.read());
  }  
     
}
