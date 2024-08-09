#include "variable.h"
#include "wifi_task.h"


#if _DISPLAY
  #include "display.h"
#endif
#include "esp_util.h"

#include <WiFi.h>
// for https    
#include <WiFiClientSecure.h>      //#include <HTTPClient.h>   

#include "myeeprom.h"

#include <ESPmDNS.h>



#if _DISPLAY
extern CDisplay			display;
#endif

extern CMyEEPROM  		eeprom;

#include "myhttp.h"
CMyHttp 			myhttp;

#if _MYJSON
//#include "myjson.h"
//CMyJson         myjson;
#endif

WiFiClientSecure espClient;


#define WIFI_TIMEOUT_MS     20000

  // 하늘상태 sky : 맑음(1), 구름많음(3), 흐림(4)
  // 강수형태 pty : 없음(0), 비(1), 비/눈(2), 눈(3), 소나기(4)
const char* WEATHER[] = { "없음", "비", "비/눈", "눈", "소나기", "맑음", "XX", "구름많이", "흐림", "O", "*" };


int _pm10_level( int val )
{
    if ( val <= 15 )     return 0;
    else
    if ( val <= 30 )   return 1;
    else
    if ( val <= 40 )    return 2;
    else
    if ( val <= 50) return 3;
    else
    if ( val <= 75)    return 4;
    else
    if ( val <=100)     return 5;
    else
    if ( val <=150)    return 6;

    return 7;
}
int _pm25_level( int val )
{
    if ( val <= 8 )    return 0;
    else
    if ( val <= 15 )   return 1;
    else
    if ( val <= 20 )   return 2;
    else
    if ( val <= 25)    return 3;
    else
    if ( val <= 37)    return 4;
    else
    if ( val <= 50)   return 5;
    else
    if ( val <= 75)   return 6;
  return 7;
}

const char * level_text[] = { "최고좋아요!", 
                         "  좋아요", 
                         " 양호해요", 
                         "보통이에요", 
                         "  나빠요 ", 
                         "상당히나빠요", 
                         "매우나빠요", 
                         "최악이에요" };
const char * getDustfineText( int pm10, int pm25 )
{
  if ( _pm10_level(pm10) > _pm25_level(pm25) )
    return level_text[_pm10_level(pm10)];

  return level_text[ _pm25_level(pm25) ];
}


void CWifiTask::disp_data_(int bootCount)
{
	Serial.println("draw main page....----------------------------------");

#if _DISPLAY
  int WH = GxEPD_WHITE;
  int BK = GxEPD_BLACK;
 // int RD = GxEPD_RED;

  bool inverse = false;
  if ( inverse) 
  {
       WH = GxEPD_BLACK; BK = GxEPD_WHITE;
  //     RD = GxEPD_WHITE;
    	display.setTextColor( GxEPD_BLACK);
    	display.fillRect(0, 10, 104, 190 );   //
  }

  // display temp 
	//epaper_.drawExampleBitmap(icon_temp, 0, 16, 32,32, WH);
	epaper_.drawExampleBitmap(icon_sunny, 10+70, 10+16, 32,32, WH);

  /** */
  const unsigned char *imgFineDust[] = { 
    icon_finedust_0, icon_finedust_1, icon_finedust_2, icon_finedust_3, 
    icon_finedust_4, icon_finedust_5, icon_finedust_6, icon_finedust_7 
  };

  int pm10_lev = _pm10_level( eeprom.pm10 );
  int pm25_lev = _pm10_level( eeprom.pm25 );
  epaper_.drawExampleBitmap( imgFineDust[   ( pm10_lev > pm25_lev ?pm10_lev : pm25_lev) ], 
                              122/2-104/2, 250/2-100/2, 104, 100, WH);


	//비(1), 비/눈(2), 눈(3), 소나기(4), 맑음(5), 구름많이(7), 흐림(8)
	char buf[0x7f];
//	sprintf( buf, "날씨: %s",  WEATHER[ eeprom.weather ]  );
//	display.drawHanText(6,  16, buf);
	display.setTextColor( BK );
 
  /* 기온 */
  sprintf( buf, "%d", eeprom.temp );
	display.drawHanText(32,  14 +6 + 2, buf);

  /* 습도 */
  sprintf( buf, "%d", eeprom.humidity );
	display.drawHanText(32,  14+ 6+14+3, buf );

	/**
	 * draw DATE
	 */
  display.setFontSize(2);
	display.drawText(15+70, 30+176-8, &(eeprom.szDate[3]) );		// 20year trim.
	display.drawText(15+74, 30+178, (eeprom.szTime) );

  /**
   *
   */
  display.setFontSize(12);
  display.setTextColor( BK );
	//setFontSize(24);	//엄청큼
	//setFontSize(12);

  // Dustfine Title 
  const char *pstr = getDustfineText( eeprom.pm10, eeprom.pm25);
  display.drawHanText(28, 155+20,  pstr );
  /** pm10/pm20 */
  epaper_.drawExampleBitmap(icon_pm10, 6+0,   234-34, 32,32, WH);
  epaper_.drawExampleBitmap(icon_pm25, 6+32+6,234-34, 32,32, WH);
  
	//display.setFontSize(9);
	display.drawHanText(2+6+5+5,  234-36+13,  String( eeprom.pm10 ).c_str() );
	display.drawHanText(2+6+6+37+5, 234-36+13,  String( eeprom.pm25 ).c_str() );      
#endif

  Serial.println("end of draw main page....-----------------------------");
  /* draw bottom */
  #if _DISPLAY
    //display.fillRect( 0, 200 -2 , 28,14 );
	  //display.drawHorzLine( 0, 200-2, 104, BK);
	  display.setTextColor( WH);  display.drawHanText(4,  236, "측정");
	  display.setTextColor( BK);  display.drawHanText(36, 236, eeprom.mLocation.c_str() );

    /* Times  */
    //display.setFontSize(1);
    char buff[20];
	  sprintf(buff, "%d", bootCount );
  	display.drawHanText(112, 236, buff);   
  #endif 
}
/**
 * 
 */
void CWifiTask::display_firm()
{
#if _DISPLAY
  display.setTextColor( GxEPD_RED );
	display.drawText(2, 10, "FIRMWARE");
	display.drawText(2, 22, "UPDATER" );
         
	int ypos = 80;
	display.drawHanText(6, ypos,    "유지보수를");
	display.drawHanText(6, ypos+12, "시작합니다");
	//drawHanText(6, 164, ssid.c_str());
    //drawHanText(6, 176, pass.c_str());
#endif 
}

void 
CWifiTask::display_wifi_not_configuration(String ssid, String pass)
{
#if _DISPLAY
  display.setTextColor( GxEPD_BLACK );
	display.drawText(2, 10, "WIFI");
	display.drawText(2, 22, "CONNECTING FAILED" );
	
  int ypos = 80;
	display.drawHanText(16, ypos,    "와이파이 정보가");
	display.drawHanText(16, ypos+12, "설정되지 않았습");
	display.drawHanText(16, ypos+24, "니다.리셋버튼을");
	display.drawHanText(16, ypos+36, "누르고 앱연결후");
	display.drawHanText(16, ypos+48, "와이파이 설정을");
	display.drawHanText(16, ypos+60, "완료해주세요");
  //drawHanText(6, 164, ssid.c_str());
  //drawHanText(6, 176, pass.c_str());
#endif
}

void 
CWifiTask::display_wifi_connection_faied( String ssid, String pass)
{
#if _DISPLAY
  display.setTextColor( GxEPD_RED );
	display.drawText(12, 10, "WIFI");
	display.drawText(12, 22, "CONNECTING FAILED" );   

	int ypos = 80;
	display.drawHanText(16, ypos,    "와이파이 연결에");
	display.drawHanText(16, ypos+12, "실패하였습니다.");
	
	display.drawHanText(16, ypos+24, "와이파이  연결");
	display.drawHanText(16, ypos+36, "정보가 정확한지");
	display.drawHanText(16, ypos+48, "확인해 주세요.");
	display.drawHanText(16, ypos+60, "완료해주세요");	
	
	display.drawHanText(16, ypos+90, ssid.c_str());
  display.drawHanText(16, ypos+102, pass.c_str());
#endif
}

/**
 * 
 */
void CWifiTask::display_statusbar()
{
#if _DISPLAY
	//display.fillRect(0, 0, 104, 12 );
	int rssi = WiFi.RSSI();
  const unsigned char * icon_rssi = rssi_fail; 
		if ( rssi >= -60 )    icon_rssi = rssi_excellent;
		else
		if ( rssi >= -80 )    icon_rssi = rssi_good;
		else
		if ( rssi >= -94 )    icon_rssi = rssi_mid;
  epaper_.drawExampleBitmap( icon_rssi, 4, 0, 16, 14, GxEPD_BLACK);
  {
    display.setFontSize(2);
		display.setTextColor( GxEPD_WHITE);
		char buff[20];
		sprintf(buff, "%d", WiFi.RSSI() );
		display.drawText(22, -4, buff);
	}
  /** Draw Battery Level */
  const unsigned char * pImg = battery_lev();
  epaper_.drawExampleBitmap( pImg, 80 +2, 1+1, 16,10, GxEPD_BLACK);
#endif
}

/**
 * 
 */
void CWifiTask::display_main(int bootCount )
{	
#if _DISPLAY

  /** draw bg */
  epaper_.drawExampleBitmap(gImage_bg, 0, 0, 122/*104*/, 250/*212*/, GxEPD_WHITE ); 

	//Serial.println();
	//display.fillScreen(GxEPD_WHITE);
  //display.setTextColor(GxEPD_BLACK);
	disp_data_(bootCount);
  display_statusbar();      // overlay

//display.drawExampleBitmap(gImage_header, 0, 0, GxEPD_WIDTH, 52, GxEPD_BLACK);                                                                                                                                                                                                                                  
	//drawText(2, 60, "WIFI");
	//drawText(2, 82, "CONNECTING" );
	//drawText(2, 102, ssid.c_str() );
	//display.update();	
/*                
	PRINT("Connected with IP: ", WiFi.localIP());
	drawText(2, 92, WiFi.localIP().toString().c_str() );
	drawText(2, 112, "Success!!" );
*/
//	epaper_.drawExampleBitmap(gImage_firebase, 70, 102, 32,32, GxEPD_RED);
#endif                   
}

/**
 * 
 */
void messageHandler(char* topic, byte* payload, unsigned int length)
{
	Serial.println("incoming: " +  String(topic) );
/*
    StaticJsonDocument<200> doc;
	  deserializeJson(doc, payload);

	const char * pmsg_l = doc["message"];
	const char * pcmd = doc["cmd"];
	if ( pmsg_l != 0 )
	{
		String message_l = String(pmsg_l);
		Serial.println( "   Received msg : " + message_l );
	}
 // int32_t usecText = textMessageOut( message_l );
       //serialOut(F("Text\t"), usecText, 5000, true);
	if ( pcmd  != 0 )   {
		Serial.println( "	Received cmd : " + String(pcmd) );
		CAws::parse_iot_cmd((char*)pcmd, "" );  
	}
	if ( pmsg_l != 0 )	{
		//screen.SetType(5);
        screen.SetPopupMessage ( String(pmsg_l) );
		aws.mMsg = true;
	}*/
} 


/**
 * 
 */
void CWifiTask::start(String ssid, String pass, int bootCount)
{
	if(ssid.length() > 0 && pass.length() > 0)
	{
      PRINT("Connecting to Wifi : ",  ssid );    
      PRINT( "WIFI : ",  "'" + ssid + "'" );
      PRINT( "Pass : ",  "'" + pass + "'" ); 
   
      //WiFi.mode(WIFI_STA);
      WiFi.begin( ssid.c_str(), pass.c_str() );
      Serial.println("");
      Serial.println( "Connecting to Wi-Fi : " + eeprom.mSSID );  
      unsigned long startAttempttime = millis();
      while(WiFi.status() != WL_CONNECTED && (millis() - startAttempttime<WIFI_TIMEOUT_MS) ){
          delay(500);
          Serial.print(".");
      }
      Serial.println( "\nMAC Addr : " + WiFi.macAddress());
			
        //mpClient = new PubSubClient(espClient);
			  //mpClient->setSocketTimeout(1); // 1 sec
      if ( WiFi.status() != WL_CONNECTED) 
      {
				        Serial.println("\nFailed to Connect to Wifi");
                display_wifi_connection_faied( ssid,  pass);
      }
      else
      {
                Serial.println("");
                Serial.println("Connected to " + eeprom.mSSID );
                Serial.print("IP address: " );
                Serial.println( WiFi.localIP()); 
                Serial.println("");      



  // Initialize mDNS
  if (!MDNS.begin("titanic")) {   // Set the hostname to "esp32.local"
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
  }
    Serial.println("mDNS responder started");




                espClient.setCACert(AWS_CERT_CA);
                //espClient.setCertificate(AWS_CERT_CRT);
                //espClient.setPrivateKey(AWS_CERT_PRIVATE);	// for client verification
                //espClient.connect( "https://api.p2sg.link/api/p2sg/localtime", 443);
                myhttp.begin( &espClient );
                myhttp.fetch();
        		/* */
                  display_main(bootCount);      	  
      }
	}
	else
	{
    display_wifi_not_configuration(ssid, pass);
	}
#if _DISPLAY
    display.update();
#endif
}
