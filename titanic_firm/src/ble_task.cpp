#include "variable.h"
#include "ble_task.h"
#if _DISPLAY
#include "display.h"
#endif

#include "esp_util.h"
#include "myeeprom.h"




#if _DISPLAY
extern CDisplay			display;
#endif
extern CMyEEPROM  		eeprom;



//#include GxEPD_BitmapExamples
//#include GxEPD_BitmapWaveshare


CBleTask::	CBleTask()
	{

		
	}
CBleTask:: ~CBleTask()
	{
		
	}




bool CBleTask::deviceConnected = false;
BLECharacteristic *pCharacteristic = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      CBleTask::deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      CBleTask::deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks 
{


  String getValue(String data, char separator, int index){
		int found = 0;
		int strIndex[] = {0, -1};
		int maxIndex = data.length()-1;
    
		for(int i=0; i<=maxIndex && found <=index; i++){
			if(data.charAt(i)==separator || i==maxIndex){
				found++;
				strIndex[0] = strIndex[1]+1;
				strIndex[1] = (i==maxIndex) ? i+1 : i;
			}
		}
		return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
	}

	void onWrite(BLECharacteristic *pCharacteristic)
	{
		//ppp std::string value = pCharacteristic->getValue();
    std::string value = pCharacteristic->getValue();

		if(value.length() > 0){
			PRINT("Value : ", value.c_str());
			//if ( value.substr(text.length()-1, text.length()) == '\n' )
			//value.erase(value.find('\n'));
			//value.replace( value.find('\n'), '\0' );
			//writeString(wifiAddr, value.c_str());


			String wifiName = getValue( String(value.c_str()), ',', 0);
			String wifiPass = getValue( String(value.c_str()), ',', 1);
      String Location = getValue( String(value.c_str()), ',', 2);

			eeprom.writeEEPROM( ADDR_SSID,	wifiName.c_str() );   
			eeprom.writeEEPROM( ADDR_PASS,	wifiPass.c_str() );   
			eeprom.writeEEPROM( ADDR_LOCATION, Location.c_str() );


      PRINT( "LCOATION", Location.c_str()  );

      esp_restart_now();

		}
	}
  /*
/*
    if(value.length() > 0){
      PRINT("Value : ", value.c_str());
      //if ( value.substr(text.length()-1, text.length()) == '\n' )
      //value.erase(value.find('\n'));
      //value.replace( value.find('\n'), '\0' );
      writeString(wifiAddr, value.c_str());
      esp_restart_now();
    }
    */
 // }

  /*
  void writeString(int add, String data){
    int _size = data.length();
    for(int i=0; i<_size; i++)
    {
      EEPROM.write(add+i, data[i]);
    }
    EEPROM.write(add+_size, '\0');
    EEPROM.commit();
  }*/
};



void CBleTask::display_statusbar()
{
#if _DISPLAY
	// display.fillRect(0, 0, 104, 12 );
	  
  const unsigned char  * pImg =  battery_lev();
  if ( icon_bat_20 == pImg )
    epaper_.drawExampleBitmap( pImg, 80 +2, 1+2, 16,8, GxEPD_RED);
  else
    epaper_.drawExampleBitmap( pImg, 80 +2, 1+2, 16,8, GxEPD_BLACK);
#endif
}



void CBleTask::start()
{
	
	Serial.println("do draw 0");


#if _DISPLAY
    display.fillScreen(GxEPD_WHITE);

          Serial.println("do draw 1");
    //disp_.drawExampleBitmap(gImage_firebase, 34, 10, 32,32, GxEPD_RED);

    display_statusbar();

    display.setFontSize(9);
    display.setTextColor( GxEPD_RED );
    display.drawText(6, 40, "BLE MODE ");

    /** */
    display.drawRectShadow( 10, 80, 100, 60, GxEPD_BLACK);

    display.setTextColor( GxEPD_BLACK );
    display.drawHanText(12, 82, "앱으로 다음을");
    display.drawHanText(12, 94, "연결해 주세요.");
    display.drawHanText(12, 106, "(약60초 이내)");
        
    display.setFontSize(7);
    display.setTextColor( GxEPD_BLACK );
    display.drawHanText(12, 120, eeprom.mThingName.c_str() );

    display.setTextColor( GxEPD_BLACK );
    display.drawHanText(2, 150, "선택된 AP:");
    //setFontSize(1);
    // red
	display.drawHanText(12, 162, eeprom.mSSID.c_str() );
	display.drawHanText(12, 162+12, eeprom.mPass.c_str() );	

  display.drawHanText( 12, 162+12+12,  eeprom.mLocation.c_str() );

	// display firmware version
    display.setTextColor( GxEPD_BLACK);
    display.drawHanText(56,  200, (const char*) _FIRMWARE_SIG);	

    display.update();
    Serial.println("do drawing... epd");
#endif
    
	/** BLE */
    BLEDevice::init(eeprom.mThingName.c_str() );
  
    BLEServer  *pServer = BLEDevice::createServer();
				pServer->setCallbacks(new MyServerCallbacks() );

	//
    // Create the BLE Service 
    BLEService *pService = pServer->createService(SERVICE_UUID);
				pCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID_TX,
                		BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |           
                		BLECharacteristic::PROPERTY_INDICATE | BLECharacteristic::PROPERTY_NOTIFY );
				pCharacteristic->setCallbacks(new MyCallbacks() );              
				
        // ble2902 needed to notify
				//ppp pCharacteristic->addDescriptor( new BLE2902() );
				pService->start();

	//
    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
					pAdvertising -> addServiceUUID( SERVICE_UUID );
					pAdvertising -> setScanResponse(false);
					pAdvertising->setMinPreferred( 0x0 );
  
    BLEDevice::startAdvertising();
    Serial.println("waitting a client connection to notify..");


	delay(500);

	pre_timer = millis();
	wait_time = (1000*15);	// 60 sec 
}


void CBleTask::loop()
{
	/**
	 * 
	 */
  //PRINT( "mill", millis() ) ;
  //PRINT( "compare", (pre_timer + wait_time)  );

#if 1
	if ( millis() > (pre_timer + wait_time) )
	{
      esp_restart_now();    
	}
#endif
}
