

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library


#include <SPI.h>
#include <SD.h>

#include <SoftwareSerial.h>
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


static const uint16_t PROGMEM ballcolor[4] = {
  0x0080, // Green=1
  0x0002, // Blue=1
  0x1000,  // Red=1
   0x1082  // white
};


void setup() {


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
  

/*
  Serial.print("Initializing SD card...");

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
*/
  //
  //
  //
  
  matrix.begin();
  matrix.setTextWrap(false); // Allow text to run off right edge
  matrix.setTextSize(2);


}

void loop() {
  byte i;

  // Clear background
  matrix.fillScreen(0);




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

  // Draw big scrolly text on top
  matrix.setTextColor(matrix.ColorHSV(hue, 255, 255, true));
  matrix.setCursor(textX, 8);
  matrix.print(F2(str));



  // Move text left (w/wrap), increase hue
  if((--textX) < textMin) textX = matrix.width();
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
