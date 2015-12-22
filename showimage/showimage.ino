#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library


//#include "GIFDecoder.h"

#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>



#include <avr/pgmspace.h>

//#include <qnlib.h>
uint8_t img24[32*32*3];
void qn_drawBMP(RGBmatrixPanel *matrix)
{
    
    //matrix.swapBuffers(true);
    for ( int x = 0; x <32; x++)
        for ( int y = 0; y <32; y++)
        {
            matrix->drawPixel( x, y,
                             matrix->Color888(
                                             img24[(31-y)*(32*3) + x*3],
                                             img24[(31-y)*(32*3) + x*3+1],
                                             img24[(31-y)*(32*3) + x*3+2],
                                             true    // gamma
                                             )
                             
                             );  
        }
    
}





// 
int  enumerateBMPFiles(const char *directoryName, boolean displayFilenames);
void getBMPFilenameByIndex( const char *directoryName, int index, char *pnBuf );

int numberOfFiles;

bool isAnimationFile(const char filename []) {
    if (filename[0] == '_')
        return false;

    if (filename[0] == '~')
        return false;

    if (filename[0] == '.')
        return false;

/*
    String filenameString = String(filename).toUpperCase();
    if (filenameString.endsWith(".BMP") != 1)
        return false;
        */

    return true;
}



// Enumerate and possibly display the animated GIF filenames in GIFS directory
int enumerateBMPFiles(const char *directoryName, boolean displayFilenames) {

    numberOfFiles = 0;

    File directory = SD.open(directoryName);
    if (!directory) {
        return -1;
    }

    File file = directory.openNextFile();
    while (file) {
        if (isAnimationFile(file.name())) {
            numberOfFiles++;
            if (displayFilenames) {
                Serial.println(file.name());
            }
        }
        file.close();
        file = directory.openNextFile();
    }

    file.close();
    directory.close();

    return numberOfFiles;
}

// Get the full path/filename of the GIF file with specified index
void getBMPFilenameByIndex(const char *directoryName, int index, char *pnBuffer) {

    char* filename;

    // Make sure index is in range
    if ((index < 0) || (index >= numberOfFiles))
        return;

    File directory = SD.open(directoryName);
    if (!directory)
        return;

    File file = directory.openNextFile();
    while (file && (index >= 0)) {
        filename = file.name();

        if (isAnimationFile(file.name())) {
            index--;

            // Copy the directory name into the pathname buffer
            strcpy(pnBuffer, directoryName);

            // Append the filename to the pathname
            strcat(pnBuffer, filename);
        }

        file.close();
        file = directory.openNextFile();
    }

    file.close();
    directory.close();
}



#define DISPLAY_TIME_SECONDS  (10)
// range 0-255
const int defaultBrightness = 255;

#define SD_CS    (53)    // 7
#define GIF_DIRECTORY "/gifs/"
#define BMP_DIRECTORY "/plat_lg/"
//#define BMP_DIRECTORY "/abc/"


int num_files;
const int buttonPin   = 2;

    

#define CLK 11  // MUST be on PORTB! (Use pin 11 on Mega)
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false/*, 32*/);

void screenClearCallback(void) {

    //matrix.fillScreen({0,0,0});
    matrix.fillScreen( 0 );
}

void updateScreenCallback(void) {
    matrix.swapBuffers( true );
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
     // matrix.drawPixel(x, y, {red, green, blue});
      matrix.drawPixel(x, y, matrix.Color888(red,green, blue ) );
}


volatile byte flag0 = LOW; // declare IRQ flag
volatile byte flag1 = LOW; // declare IRQ flag




void setup() {
    int     i, len;
    uint8_t *ptr = matrix.backBuffer(); // Get address of matrix data

  
  // Copy image from PROGMEM to matrix buffer:
   // memcpy_P(ptr, img, sizeof(img));

/*
    setScreenClearCallback(screenClearCallback);
    setUpdateScreenCallback(updateScreenCallback);
    setDrawPixelCallback(drawPixelCallback);
*/
    
  // Start up matrix AFTER data is copied.  The RGBmatrixPanel
  // interrupt code ties up about 40% ofk the CPU time, so starting
  // it now allows the prior drawing code to run even faster!
  matrix.begin();
  //matrix.setBrightness( defaultBrightness );

 // Clear screen
    matrix.fillScreen( 0 );
    matrix.swapBuffers(true);
 
    matrix.setTextSize(1);
    matrix.setTextWrap(true); // Allow text to run off right edge

	Serial.begin(9600);


    // initialize the SD card at full speed
    pinMode(SD_CS, OUTPUT);
    if (!SD.begin(SD_CS)) {
        //matrix.scrollText("No SD card", -1);
        matrix.print("no sd card");
        Serial.println("No SD card");
        while(1);
    }


    num_files = enumerateBMPFiles(BMP_DIRECTORY, false );
    if ( num_files < 0 )
    {
        matrix.print("no BMPs directory");
        Serial.println("No BMPs directory");
        while(1);
    }
    if(!num_files) {
        //matrix.scrollText("Empty gifs directory", -1);
        matrix.print( "empty BMPs directory" );
        Serial.println("Empty BMPs directory");
        while(1);
    }
    

/*

  //pinMode( buttonPin, INPUT);


  attachInterrupt( digitalPinToInterrupt(buttonPin), mode_isr0, CHANGE);  
  attachInterrupt(1, mode_isr1, RISING); 
  */

}

void mode_isr0()
{
 Serial.println("inttruppt....");
}
void mode_isr1()
{

  
/*
 * 
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    pushed = true;
  } else {

    if ( pushed == true )
   */ 
    {
           Serial.println("pushed....");
    //  pushed = false;
      num_files = enumerateBMPFiles("/abc/", false );
    }


}


void processBMPFile( char * szFile )
{
  File myFile = SD.open(szFile, FILE_READ);
  int inx = 0;
  if ( myFile == false)
  {
        matrix.print("open err");
        Serial.println("No SD card");
    
        return;
  
  }
  else
  {
    Serial.println("read start...");
    if ( myFile.available())
         for ( int i  = 0; i < 0x37 -1; i++)
              myFile.read();


   int line = 0;
    while ( myFile.available() ) 
    {
          unsigned char b = myFile.read();
          unsigned char g = myFile.read();
          unsigned char r = myFile.read() ;          


        line += 3;
        if ( line >= 96 )   // skip  over 32pixel 
        {
          /* myFile.read();
              myFile.read();
              myFile.read();
          */             
          line = 0;
        }
 

        img24[ inx    ] = r;
        img24[ inx +1 ] = g;
        img24[ inx +2 ] = b;
        inx += 3;

        if ( inx >= 32*32*3 )
            goto EX;
      
    }
 }
EX:
    myFile.close();
    Serial.println("read ok...");

}


void loop() {
  
	char pathname[30];
	static int index = random(num_files);
  matrix.swapBuffers(true);    
	getBMPFilenameByIndex(BMP_DIRECTORY, index++, pathname);
	if (index >= num_files) 
		index = 0;
	Serial.println(pathname);
	processBMPFile(pathname);
  qn_drawBMP(&matrix);
	delay(3000);
}
