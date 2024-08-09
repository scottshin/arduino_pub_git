
#pragma 

//#include <HardwareSerial.h> // 하드웨어 시리얼 라이브러리


#define FIRM_VER  (20)
#define _FIRMWARE_SIG  (F("v@0.2.1"))
#define BOARD_NAME  (F("QN-FINEDUST ")) 

#define PROMPT     (("CMD>"))
#define ACK        (("ack"))          //  F 로 묶으면 안됨 
#define help_msg  F(" ## Command List :" \
					"\n cmd> reboot" \
					"\n cmd> dhcp:[true:false]" \
					"\n      ip:192.168.0.100" \
					"\n      dns:192.168.0.1" \
					"\n      gw:192.168.0.1" \
					"\n      mask:255.255.255.0" \
					"\n cmd> read         #read LED Status" \
					"\n cmd> ssid:SK_WiFiGIGA3506_2.4G" \
					"\n      pass:BADB7@5046" \
					"\n")

#define _WEB_UPDATE            (1)     // web update   // pg code 100k 


#define DNS             (0)     // 0x6588 (25992)
#define _DEBUG_FONT		  (0)
#define _FORCE_REBOOT   (1)

#define _DISPLAY        (0)

#define  _SPIFFS		(0)

/* WIFI만  68% */
#define _WIFI           (1)     // pg code 400000 byte



/* WIFI + BLE  127% */
#define _BLE_TASK       (1)     // pg code 754,952 byte

#define _AWS            (0)     // pg code veryhuge  ( Pub )

// EEPROM 
#define EEPROM_SIZE   127   // ESP32 can use up to 512 byte in the flash memory

// ESP32 
#define uS_TO_S_FACTOR 1000000UL  /* Conversion factor for micro seconds to seconds */

#define ONE_MIN           60
#define FIFTEEN           900
#define ONE_HOUR          3600
#define FIVE_MIN			(ONE_MIN*5)
#define FIVE_THIRTY			(30)
#define TIME_TO_SLEEP  		(FIFTEEN)     // 300:20시간   /* Time ESP32 will go to sleep (in seconds) */  //60 = 1min , 900 = 15min, 3600000 = 1H


// EEPROM INDEX
//#define ADDR_THINGNAME 	0
#define ADDR_LOCATION	(0)
#define ADDR_OWNERID	20
#define ADDR_SSID 		40
#define ADDR_PASS     65
#define ADDR_NETTYPE	94
// variable
#define ADDR_DHCP 			100
#define ADDR_LOG  			101
#define ADDR_MODE       102

/* EEPROM String address */
#define ADDR_IP             108
#define ADDR_GW             115

#define PRINT( x, y )        { Serial.print( x ); Serial.println( y ); }
#define PRINTX( x, y, p, z ) { Serial.print( x ); Serial.print( y ); Serial.print( p ); Serial.println( z ); }



