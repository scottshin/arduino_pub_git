//
// Show Image
//


#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library


//#include "GIFDecoder.h"

#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

#define HC06 Serial3

#include <avr/pgmspace.h>

#include <EEPROM.h>


//#include <qnlib.h>
uint8_t img24[32*32*3];
void qn_drawBMP(RGBmatrixPanel *matrix)
{
    //matrix.swapBuffers(true);
    for ( int x = 0; x <32; x++)
        for ( int y = 0; y <32; y++)
        {
            matrix->drawPixel( x, y,  matrix->Color888(   img24[(31-y)*(32*3) + x*3], img24[(31-y)*(32*3) + x*3+1], img24[(31-y)*(32*3) + x*3+2],
                                                 true    // gamma
                                             )
            );  
        }
}


// 
int  enumerateBMPFiles(const char *directoryName, boolean displayFilenames);
void getBMPFilenameByIndex( const char *directoryName, int index, char *pnBuf );

#define DISPLAY_TIME_SECONDS  (10)
// range 0-255
const int defaultBrightness = 255;

#define SD_CS    (53)    // 7
#define GIF_DIRECTORY "/gifs/"
//#define BMP_DIRECTORY "/coff/"
//#define BMP_DIRECTORY "/abc/"

char * BMP_DIRECTORY = "/plat_lg/";

int num_files;
const int buttonPin   = 8;

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

int nRotation = 0;

void setup() {
    int     i, len;
    uint8_t *ptr = matrix.backBuffer(); // Get address of matrix data

    Serial.println("reboot!");

  
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
    while(!Serial) {
        ;
    }

    HC06.begin(9600); // set the data rate for the BT port
    Serial.println("bluetooth send on!");

    
/*
  delay(100);  // Short delay, wait for the Mate to send back CMD
  HC06.println("U,115200,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  HC06.begin(115200);  // S
*/
    // initialize the SD card at full speed
    pinMode(SD_CS, OUTPUT);
    if (!SD.begin(SD_CS)) {
        //matrix.scrollText("No SD card", -1);
        matrix.print("no sd card");
        Serial.println("No SD card");
        while(1);
    }

#if 1
    num_files = enumerateBMPFiles(BMP_DIRECTORY, false );
    if ( num_files < 0 )
    {
        matrix.print("no BMPs directory");
        Serial.print(BMP_DIRECTORY );
        Serial.println("No BMPs directory");
        while(1);
    }
    if(!num_files) {
        //matrix.scrollText("Empty gifs directory", -1);
        matrix.print( "empty BMPs directory" );
        Serial.print(BMP_DIRECTORY );
        Serial.println("Empty BMPs directory");
        while(1);
    }
#endif


    
  nRotation = EEPROM.read( 0);
  matrix.setRotation(3/*nRotation*/ );

#if 1
  attachInterrupt( digitalPinToInterrupt(2), mode_isr1, FALLING);  
 // attachInterrupt(digitalPinToInterrupt(3), mode_isr1, FALLING);   // 1 is digital(3)
#endif

    matrix.print("Boot Okay!");
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

    
    Serial.println("pushed....");
    //  pushed = false;
    BMP_DIRECTORY = "/abc/";
    num_files = enumerateBMPFiles(BMP_DIRECTORY, false );
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

/**
 *  
 */
void btCmdParser()
{
    Serial.println("btParser...");
    SD.remove("sendfile.bmp");

    File myFile; 
 
    int step = 0;
    int inx = 0;

    char cmd[20];
    char file[20];
    char stream[1024];

    memset ( cmd, 0, 20 );
    memset ( file, 0, 20 );
  
    char data;
    int empty_count = 0;
    int stream_byte = 0;
    do {
        data = 0;
        if ( HC06.available() )
        {
            empty_count = 0;
            data = HC06.read();
            if(data == -1)
                Serial.println( "minus data");  
            switch ( step )
            {
                case 0: if ( data == '[' )
                            step++;
                        inx = 0;
                        break;
                case 1:  //cmd 
                        cmd[inx++] = data;
                        if ( inx >=4 )  {
                            Serial.print("CMD : ");
                            Serial.println(cmd);
                            step++;
                            inx = 0;
                        }
                        break;

                case 2:  // filename 
                        file[inx++] = data;
                        if ( inx >= 12)  {
                            Serial.print("FILE: ");
                            Serial.println(file);
                            inx = 0;
                            step++;
                            myFile = SD.open( "sendfile.bmp", FILE_WRITE);
                            matrix.print("File Recv.");
                        }
                        break;
                case 3:  // data
                    {
                        stream[stream_byte++] = data;
                        if ( inx++ % 127 == 0 )   {
                            Serial.print("INX         =" );
                            Serial.println( inx, DEC);
                        }
                        if ( stream_byte>= 1024)
                        {
                            myFile.write( stream,1024);
                            stream_byte = 0;
                        }
                    }
                    break;
            }
        }
        else
        {
            Serial.println("empty data");
            if ( empty_count++ > 5 )
            {
                data = -1;
                Serial.print("total copy : " );
                Serial.println(inx );

                myFile.write( stream, stream_byte);
                myFile.close();
            }
            delay(200);
        }
    } while ( data != -1 );
   
    Serial.println("exit");
}


void loop() {

/*
	char pathname[30];
	static int index = random(num_files);
  matrix.swapBuffers(true);    
	getBMPFilenameByIndex(BMP_DIRECTORY, index++, pathname);

	if (index >= num_files) 
		index = 0;
	Serial.println(pathname);
	processBMPFile(pathname);
  qn_drawBMP(&matrix);
	delay(1000);
*/

  char data;
  while(HC06.available()) {
      data = HC06.read();
      if(data == -1) break;
      if ( data == '@' )
      {
        Serial.println("BTcmd!");
        
        btCmdParser();
        
        processBMPFile( "sendfile.bmp" );
        qn_drawBMP(&matrix);
       
      }
      else
      {
          // skip
      }
   }  

   
}
