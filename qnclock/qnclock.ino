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

#include <EEPROM.h>

#define HC06 Serial3


File myFile;


tmElements_t tm;
tmElements_t tm_write;
int write_flag = 0;

  

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr


#define SD_CS    (53)    // 7

#define CLK      (11)  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT      (10)
#define OE       (9)


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
  {3,0, -1, -1 },  // 3
  {4,0, -1. -1 },  // 4
  {1,0, 1, 1},  // 5
  {0,1, 1, 1},  // 6
  {2,0, 2, 1},  // 7
  {0,1, 0, 2},  // 8
  {1,2, 2, 2},  // 9
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



#define GREEN     0
#define BLUE      1
#define RED       2
#define WHITE     3
#define GRAY      4
#define YELLOW    5

static const uint16_t PROGMEM ballcolor[6] = {
  0x0080, // Green=1
  0x0002,  // Blue=1
  0x1000,  // Red=1
   0x1082,  // white
   
   0x1052,   // GRAY

   0x1080  // YELLOW
};

struct PIXEL {
  int x;
  int y;
};

PIXEL pxHour;
PIXEL pxMin;


int nRotation = 0;


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
  //e
  
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(2);

  nRotation = EEPROM.read( 0);
  matrix.setRotation( nRotation );

#if 1
  attachInterrupt( digitalPinToInterrupt(2), mode_isr0, FALLING);  
  attachInterrupt(digitalPinToInterrupt(3), mode_isr1, FALLING);   // 1 is digital(3)
#endif

}


void mode_isr0()
{
    Serial.println("rotation....");
 
    nRotation = ++nRotation %4;
    matrix.setRotation( nRotation );
      
    EEPROM.write( 0, nRotation );
  
}
void mode_isr1()
{
  // set_time



  tm_write = tm;
  


    if ( tm_write.Minute < 55 )
    {
      tm_write.Minute += 5;
    }
    else
    {
      tm_write.Hour = (++tm_write.Hour) %24;
      tm_write.Minute = 0;
    }
  //   RTC.write(tm);


    write_flag = 1;

         Serial.println("time ....");
              Serial.println(tm_write.Hour);
           Serial.println(tm_write.Minute);

 
}


#define ROUND   (2)
//#define PITCH(X)  ( X*6 + 3) 
#define BLOCK   6
int PITCH(int x )
{
    return  1 + (BLOCK/2) + (BLOCK*x);
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

    int x = (hour_xy[hour][0]);
    int y = (hour_xy[hour][1]);
    calc( x, y, pxHour.x, pxHour.y );
    fill_circle( pxHour.x, pxHour.y, WHITE);

      
    if ( hour_xy[hour][2] != -1 )
      {
          fill_circle( (hour_xy[hour][2]), (hour_xy[hour][3]),   WHITE );
      }

      // SI
      fill_circle( (4), (2), GRAY );
}

void display_min( int _min )
{
    int min = (_min /5);  
     if ( min_ten[min][0] != -1)
     {
          
         int x = (min_ten[min][0]);
         int y = (min_ten[min][1]);

          calc( x, y, pxMin.x, pxMin.y );
          fill_circle( pxMin.x, pxMin.y,   WHITE);
          
     }
     
     if ( min_ten[min][2] != -1 )
     {
          fill_circle( (min_ten[min][2]), (min_ten[min][3]),   WHITE );
     }

      if ( min_ten[min][4] != -1 )
     {
          fill_circle( (min_ten[min][4]), (min_ten[min][5]),   WHITE );
     }
    if ( min != 0 )
    {   
    // bun
      fill_circle( (4),(4),  GRAY );
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
  if ( hour == 99 )
  {
    fill_circle( (1),3, RED );
  }
  else
    fill_circle( (0), ( IsDay(hour) ? 0 : 3), IsDay(hour)? GREEN :YELLOW );
}


void fill_circle( int x, int y, int color)
{
  /*
    //matrix.fillCircle( PITCH(x), PITCH(y),   ROUND, pgm_read_word(&ballcolor[color]));
  matrix.fillCircle( PITCH(x)-1, PITCH(y),   ROUND, pgm_read_word(&ballcolor[color]));
  matrix.fillCircle( PITCH(x)+1, PITCH(y),   ROUND, pgm_read_word(&ballcolor[color]));
   matrix.fillCircle( PITCH(x), PITCH(y)-1,   ROUND, pgm_read_word(&ballcolor[color]));
   matrix.fillCircle( PITCH(x), PITCH(y)+1,   ROUND, pgm_read_word(&ballcolor[color]));
*/
  // matrix.fillCircle( PITCH(x), PITCH(y)-1,   ROUND, pgm_read_word(&ballcolor[color]));
   matrix.fillCircle( PITCH(x)-1, PITCH(y),   ROUND, pgm_read_word(&ballcolor[color]));
   matrix.fillCircle( PITCH(x), PITCH(y),   ROUND, pgm_read_word(&ballcolor[color]));
}
void display_dot( int x, int y )
{
   fill_circle( x, y, GREEN );
}

void loop() {
  byte i;

  // Clear background
  matrix.fillScreen(0);



  if ( write_flag )
  {
      RTC.write(tm_write);
      write_flag = 0;
  }


 
  if ( RTC.read(tm) )
  {
      int h = tm.Hour;
      if ( tm.Hour >= 12 )
        h = tm.Hour -12;
      display_day( tm.Hour );
      display_hour( h );
      display_min(  tm.Minute );

//      Serial.println("readed.. " );
//        Serial.println(tm.Hour );
//          Serial.println(h );
  }
  else
  {
      tm.Hour = 22; 
      tm.Minute = 10;
      RTC.write(tm);
      display_day( 99 );

      Serial.println("read fail ....");
  }

 // fill_circle(1,3, RED);

 
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
