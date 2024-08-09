/***********************************************************
 * 
 * 
 * *********************************************************/
#include <Arduino.h>

#include "variable.h"
#include "esp_util.h"

#if _BLE_TASK
  #include "ble_task.h"
#endif

#if _WIFI
  #include "wifi_task.h"
#endif 
#include "myeeprom.h"

#if _SPIFFS	
  #include "SPIFFS.h"
#endif 

#include <Adafruit_NeoPixel.h>

// Cannot use 6 as output for ESP. Pins 6-11 are connected to SPI flash. Use 16 instead.
//#define LED_PIN     13
#define LED_PIN     4 
#define LED_PIN_23     23
#define LED_PIN_D16     16



// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  35 

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN_23, NEO_GRB + NEO_KHZ800);
#if _WEB_UPDATE
  #include <AsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <AsyncElegantOTA.h>
  AsyncWebServer server(80);
#endif




#if _BLE_TASK
  CBleTask	bleTask;
#endif 
#if _WIFI
  CWifiTask	wifiTask;
#endif
  CMyEEPROM	eeprom;


unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 5000;   // Pattern Interval (ms)
bool          patternComplete = false;

int           pixelInterval = 50;       // Pixel Interval (ms)
int           pixelQueue = 0;           // Pattern Pixel Queue
int           pixelCycle = 0;           // Pattern Pixel Cycle
uint16_t      pixelNumber = LED_COUNT;  // Total Number of Pixels

void parse_command(char *cmd);

extern uint32_t Wheel(byte WheelPos);
extern void colorWipe(uint32_t color, int wait);
extern void theaterChase(uint32_t color, int wait) ;
extern void rainbow(uint8_t wait);
extern void theaterChase(uint32_t color, int wait);
extern void theaterChaseRainbow(uint8_t wait);

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
<title>ESP Input Form</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
</head><body>


<form action="/get">
input1: <input type="text" name="input1">

<input type="submit" value="Submit">

  <img src="sun">
  <img src="sun-cloud">
  <img src="cloud">
  <img src="rain">
  <img src="storm">
  <img src="snow">
  


</form>
</body></html>)rawliteral";

void setup()
{
    Serial.begin(115200);
    while(!Serial) {; }

#if _SPIFFS	
    Serial.print(("SPIFFS Initialize begin........"));
    if(! SPIFFS.begin()) {
   		Serial.println("failed to Initialise SPIFFS");
  		//SPIFFS.format();
  		while(!Serial) {; }
    }
#endif
	Serial.println("..ok"); //Initialize File System 

	strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
	strip2.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)

	strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
	strip2.setBrightness(255);           // INITIALIZE NeoPixel strip object (REQUIRED)
  for ( int i = 0; i < LED_COUNT; i++)
  {
    if ( i < 19 )
      strip.setPixelColor(i,  strip.Color(200, 255, 200)); //  Set pixel's color (in RAM)
    else    // 2floor
    if ( i < 23 )
//      strip.setPixelColor(i,  strip.Color(165, 235, 244)); //  Set pixel's color (in RAM)
      strip.setPixelColor(i,  strip.Color(95, 15, 14)); //  Set pixel's color (in RAM)
    else
      strip.setPixelColor(i,  strip.Color(15, 95, 14)); //  Set pixel's color (in RAM)


	  strip.show();            // Turn OFF all pixels ASAP
    delay(200);
  }

	strip2.setPixelColor(0, strip.Color(100,100,100));           // INITIALIZE NeoPixel strip object (REQUIRED)
	strip2.setPixelColor(1, strip.Color(200,10,10));           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip2.show();




 	eeprom.init();		// with spiff
    // int modeIdx = eeprom.mMode;
	// PRINT("modeIdx : ", modeIdx);
	// eeprom.put( ADDR_MODE, modeIdx !=0 ? 0 : 1);  

#if _WIFI      
		eeprom.mSSID = "QNOTE";
		eeprom.mPass = "a123456789";
		wifiTask.start(eeprom.mSSID , eeprom.mPass, 0 );

#endif   


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //request->send(200, "text/plain", "Welcome to Titanic Controll Center\nD13 pin");
    request->send_P(200, "text/html", index_html);
  });


#if _WEB_UPDATE
  AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server.begin();
#endif 



pinMode( LED_PIN_D16, OUTPUT);

}

int interval = 1;
void loop()
{

  if ( interval > 0 )
  {
    digitalWrite(LED_PIN_D16, HIGH);
    interval = 0;

    delay(3000);
  }
  else
  {
    digitalWrite(LED_PIN_D16, LOW);
    interval = 1;

    delay(1000);
  }

  

/*
  WiFiClient client = server.available();   

  if (client) {                        
    Serial.println("New Client.");           
    String currentLine = "";               
    while (client.connected()) {            
      if (client.available()) {            
        char c = client.read();            
        Serial.write(c);                   
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            client.println();
            break;
          } else {  
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;   
        }
        if (currentLine.endsWith("GET /H")) {
          Serial.println("HIGH");
          colorWipe(strip.Color(255,0,0), 0);
          }
        if (currentLine.endsWith("GET /L")) {
          Serial.println("LOW");
          colorWipe(strip.Color(0,255,0), 0);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
*/



  return ;




	unsigned long currentMillis = millis();                     //  Update current time
	if( patternComplete || (currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
		patternComplete = false;
		patternPrevious = currentMillis;
		patternCurrent++;                                         //  Advance to next pattern
		if(patternCurrent >= 7)
			patternCurrent = 0;
	}

	if(currentMillis - pixelPrevious >= pixelInterval) {        //  Check for expired time
		pixelPrevious = currentMillis;                            //  Run current frame
    switch (patternCurrent) {
      case 7:        theaterChaseRainbow(50); /* Rainbow-enhanced theaterChase variant */ break;
      case 6:        rainbow(10); /*Flowing rainbow cycle along the whole strip */ break;
      case 5:        theaterChase(strip.Color(0, 0, 127), 50); /* Blue*/  break;
      case 4:        theaterChase(strip.Color(127, 0, 0), 50); /* Red */ break;
      case 3:        theaterChase(strip.Color(127, 127, 127), 50); /* White */ break;
      case 2:        colorWipe(strip.Color(0, 0, 255), 50); /*Blue*/  break;
      case 1:        colorWipe(strip.Color(0, 255, 0), 50); /* Green*/        break;        
      default:       colorWipe(strip.Color(255, 0, 0), 50); /* Red*/ break;
    }
  }



if ( bleTask.deviceConnected )
{
	
}

#if 1
/*
#if _BLE_TASK  
    Serial.println(F("in loop ble"));
    if ( bleTask.deviceConnected )
    {
          // setup model 
          txValue = random(-10, 20);
          char txString[8];
          dtostrf(txValue, 1, 2, txString);
    
          pCharacteristic -> setValue (txString);
          pCharacteristic -> notify();
          Serial.println("sent value.." );
          delay(500);
    }
    else
    {
       bleTask.loop();
    }
#endif        
*/ 

/*
	bool console_mode = false;
	if(Serial.available() )
	{
  	  	//시리얼0 명령어 입력
        String cmd = Serial.readStringUntil('\n');
        PRINT(F(" key input :"), cmd );
        const int len = cmd.length() + 1;
        {
        	char cmd_array[len];
        	cmd.toCharArray(cmd_array, len);
        	//Serial.println("Nice to meet you, " + cmd + "!");
        	parse_command( cmd_array );
        }
	}
	*/
#endif

}
void cmd_files()
{

#if _SPIFFS	
	Serial.println("==========================================");
    PRINT( F(" Total bytes : "), String(SPIFFS.totalBytes() ) );
    PRINT( F(" Used bytes : "), String(SPIFFS.usedBytes() ) );
	Serial.println("------------------------------------------");

	File root = SPIFFS.open("/");
	File file = root.openNextFile();
	while(file) {
		PRINT( F("   FILE: "), file.name() );
		file = root.openNextFile();
	}
	Serial.println("==========================================");

#endif 
}

void show_info()
{
	PRINT(F("  ###################################################"), "#" );
	PRINT(F("  ##  ThingName: "), eeprom.mThingName   );
	PRINT(F("  ##  BoardName: "), String(BOARD_NAME)  );
	PRINT(F("  ##  Firmware : "), FIRM_VER );
	PRINT(F("  ###################################################"), "#" );
  PRINT(F("\n   <<Read Status>>"), "" );
  PRINT(F("     >>  Owner  ID: "), eeprom.mOwnerID );
	PRINT(F("\n   <<Network Profile>>"), "");
  PRINT(F("     >>  NETWORK ONLINE  : "), eeprom.mEthConnected );
		PRINT(F("     >>  NetType(1:WIFI,2:ETH)  : "), eeprom.mNetworkType );
		PRINT(F("     >>  Wifi SSID: "), eeprom.mSSID );
		PRINT(F("     >>  Wifi Pass: "), eeprom.mPass );
#if _WIFI    
		PRINT(F("     >>  Wifi IP: "), WiFi.localIP() );
#endif    
//        PRINT(F("\n     >>  Ethernet IP addr: "), ETH.localIP());
//        PRINT(F("     >>  GATEWAY IP addr: "), ETH.gatewayIP());
        //PRINT(F("    >>  DNS IP addr: "), ETH.dnsServerIP());
//        PRINT(F("     >>  Subnet Mask addr: "), ETH.subnetMask());
		PRINT(F("     >>  isDHCP: "), eeprom.isDhcpPref() );


	PRINT(F("\n   <<Profile>>"), "");
		PRINT(F("  ##  Location : "), eeprom.mLocation );

		PRINT(F("  ##  mMode : "), eeprom.mMode );
        PRINT(F("     >>  StatusMode: "), eeprom.mStatus );
        PRINT(F("     >>  isLog: "), eeprom.isLog() );
//        PRINT(F("     >>  datetime : "), String(year()) + "-" + String(month()) + "-" + String(day()) + ", " +String(hour()) + ":" + String(minute()) + ":" + String(second())        );

        PRINT(F("\n   <<Patch Data>>"), "");
		PRINTX( F("     >>  pm10 : "), eeprom.pm10, F(", pm2.5 : "), eeprom.pm25  );
		PRINTX( F("     >>  humidity : "), eeprom.humidity, F(", temp : "), eeprom.temp  );
		PRINT(F("     >>  weather: "), eeprom.weather );
		PRINT(F("     >>  speed-gun : "), eeprom.carspeed );

	PRINT(F("\n   <<Command List>>"), "");
		PRINT(F("     >>  download: <filename> "), "" );
		PRINT(F("     >>  files "),     F("       // file list in frash rom") );
		PRINT(F("     >>  debug "),     F("       // debug buf print") );
		PRINT(F("     >>  schedule "),  F("    // debug schedule list") );			
		PRINT(F("     >>  idle:true "), F("   // set idle status") );	
	    PRINT(F("  ---------------------------------------------------"), "-");
        PRINT(F("     >>  Free heap "), ESP.getFreeHeap() ); 
        PRINT(F(""), "");

}

/**
 * 
 */
void parse_command(char *cmd)
{
	Serial.println( cmd );
    if ( *cmd == 0xF0 )  {
        eeprom.carspeed = *(cmd +4);     // CAR SPEED    0 ~ 255km
        Serial.print(PROMPT);
        return;
    }

	char *instruct = 0;
	char *param = 0;
	char *token = strtok(cmd, ":");
	instruct = token;
	while (token != NULL) {
		param = token;
		token =strtok(NULL, ":" );
	}
    String strCmd (instruct);
    String strParam( param);
	strParam.trim();		// remove '\n'
    
    if ( strCmd. equals(F("help")))			
        Serial.println( help_msg );
    else
    if ( strCmd.equals(F("reboot")))    {
		Serial.println("will reboot");
		ESP.restart();
    } else if ( strCmd.equals(F("reset")))
    {
		//lSurgeCount =  atol(strParam.c_str()); 
		//writeLongToEEPROM(ADDR_SURGECNT, lSurgeCount);
    } else if ( strCmd.equals(F("init")))
    {
        eeprom.writeLongToEEPROM( ADDR_NETTYPE, 1  );     // 0 알수없음   1  무선랜   2 유선랜  3 lte
     
        eeprom.writeEEPROM( ADDR_LOCATION,  "광진구" );  
        eeprom.writeEEPROM( ADDR_OWNERID, "hakseong" ); 
        eeprom.writeEEPROM( ADDR_SSID, "none" ); 
        eeprom.writeEEPROM( ADDR_PASS, "none" ); 
    } else if ( strCmd.equals(F("read")))
    	show_info();
    else  
#if _SPIFFS
	if ( strCmd.equals(F("download")) )
	{
		SPIFFS.format();
  		Serial.println(F("Format complete!"));
  		/*
		http.downloadAndSaveFileSPIFFS( "/"+strParam,  "https://www.google.com/logos/doodles/2023/"+strParam );

		http.downloadAndSaveFileSPIFFS( "/ksfont12.fnt", F("https://firebasestorage.googleapis.com/v0/b/p2sg-project.appspot.com/o/ksfont12.fnt?alt=media&token=7479b5af-3245-4d32-b7f5-c6c31f5a5969") );
		http.downloadAndSaveFileSPIFFS( "/ksfont16.fnt", F("https://firebasestorage.googleapis.com/v0/b/p2sg-project.appspot.com/o/ksfont16.fnt?alt=media&token=cf11ce30-8d72-4102-a06b-effd83c18c36") );
		http.downloadAndSaveFileSPIFFS( "/utf-euc-map.dat", F("https://firebasestorage.googleapis.com/v0/b/p2sg-project.appspot.com/o/utf-euc-map.dat?alt=media&token=bcc15375-c4b0-442e-9f94-1538644cf28e") );	
		Serial.println("downloaded !");
		*/
	}
	else
#endif
	if ( strCmd.equals( F("files")) )		cmd_files();
	else
	if ( strCmd.equals( F("debug")) ) {	
	//		screen. debugBufPrint();
	} else
	if ( strCmd.equals( F("log")) )
    {
    	eeprom.put( ADDR_LOG,  atol(strParam.c_str()) );  
        PRINT(	F("  ## Stored Log status :  "), strParam  );
    }  
    else if ( strCmd.equals(F("dhcp")) )
    {
		eeprom.put( ADDR_DHCP,  atol(strParam.c_str()) );   
		PRINT(	F("  ## Stored Dhcp status : "), strParam  );
    } else if ( strCmd.equals("ip") )
	{
		eeprom.writeStringToEEPROM( ADDR_IP, strParam );     
		PRINT(	F("Stored IP Address"), strParam );
    }
    else
    if ( strCmd.equals(F("gw") ))
    {
		eeprom.writeStringToEEPROM( ADDR_GW, strParam );     
		PRINT(	F("Stored Gateway Address"), strParam );
    }
    else
    if ( strCmd.equals(F("nettype") ))
    {
    	eeprom.writeLongToEEPROM( ADDR_NETTYPE, atol( strParam.c_str())  );     // 0 알수없음   1  무선랜   2 유선랜  3 lte
		PRINT(	F("  ## Stored Nettype status : "), strParam  );
    }
    else
    if ( strCmd.equals(F("ssid") ))
    {
		eeprom.writeEEPROM( ADDR_SSID,  strParam );  
		PRINT(	F("  ## Stored SSID status : "), strParam  );
    }
    else
    if ( strCmd.equals(F("pass") ))
    {
		eeprom.writeEEPROM( ADDR_PASS,  strParam );     
		PRINT(	F("  ## Stored WIFI status : "), strParam  );
    }
	else
	if ( strCmd.equals(F("location") ))
	{
		//const char * p = strParam.c_str();
		//for ( int i =0; i < strlen(p); i++)
		//	Serial.println( p[i], HEX );
		eeprom.writeEEPROM( ADDR_LOCATION,  strParam );  
		PRINT(	F("Stored Location "), strParam);
	}
    else
    if ( strCmd.equals(F("owner") ))
    {
		eeprom.writeEEPROM(ADDR_OWNERID, strParam );     
		PRINT(	F("Stored owner id"), strParam);
    }
    else
    if ( strCmd.equals(F("ota") ))
    {
         //OTADRIVE.updateFirmware();
    }
    else
    	PRINT(	F("Unknown command : "), strParam  );
    
    Serial.print(PROMPT);
}






// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  static uint16_t current_pixel = 0;
  pixelInterval = wait;                        //  Update delay time
  strip.setPixelColor(current_pixel++, color); //  Set pixel's color (in RAM)
  strip.show();                                //  Update strip to match
  if(current_pixel >= pixelNumber) {           //  Loop the pattern from the first LED
    current_pixel = 0;
    patternComplete = true;
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  static uint32_t loop_count = 0;
  static uint16_t current_pixel = 0;

  pixelInterval = wait;                   //  Update delay time

  strip.clear();

  for(int c=current_pixel; c < pixelNumber; c += 3) {
    strip.setPixelColor(c, color);
  }
  strip.show();

  current_pixel++;
  if (current_pixel >= 3) {
    current_pixel = 0;
    loop_count++;
  }

  if (loop_count >= 10) {
    current_pixel = 0;
    loop_count = 0;
    patternComplete = true;
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   
  for(uint16_t i=0; i < pixelNumber; i++) {
    strip.setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time  
  }
  strip.show();                             //  Update strip to match
  pixelCycle++;                             //  Advance current cycle
  if(pixelCycle >= 256)
    pixelCycle = 0;                         //  Loop the cycle back to the begining
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time  
  for(int i=0; i < pixelNumber; i+=3) {
    strip.setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) % 255)); //  Update delay time  
  }
  strip.show();
  for(int i=0; i < pixelNumber; i+=3) {
    strip.setPixelColor(i + pixelQueue, strip.Color(0, 0, 0)); //  Update delay time  
  }      
  pixelQueue++;                           //  Advance current queue  
  pixelCycle++;                           //  Advance current cycle
  if(pixelQueue >= 3)
    pixelQueue = 0;                       //  Loop
  if(pixelCycle >= 256)
    pixelCycle = 0;                       //  Loop
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


