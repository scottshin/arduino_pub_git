
#include "variable.h"
#include "myeeprom.h"

#include <EEPROM.h>
#include "SPIFFS.h"

CMyEEPROM::	CMyEEPROM() {}
CMyEEPROM::	~CMyEEPROM() {}

void CMyEEPROM::init()
{
  Serial.println( "EEPROM Init...");

	mStatus = 1;    // running

        /** */
	if (!EEPROM.begin(EEPROM_SIZE) )
	{
      Serial.println("failed to initialise EEPROM" );
      
  }  /** */

	uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
	uint16_t chip = (uint16_t)(chipid >> 32);

  char ssid1[15];
  char strChipID[15];
  
	snprintf(ssid1, 15, "%04X", chip);
	snprintf(strChipID, 15, "%08X", (uint32_t)chipid);

	//Serial.println( ssid1 );
	//Serial.println( strChipID );

	String strID =  ( String(ssid1) + String(strChipID) );
	PRINT( F("  UNIT ID : "),  strID );
	mThingName = String("FD-") +  strID.substring(4,6) + strID.substring(2,4) +strID.substring(0,2);



//  mThingName = EEPROM.readString( ADDR_THINGNAME );
	mOwnerID = EEPROM.readString( ADDR_OWNERID );

	mSSID = EEPROM.readString(ADDR_SSID);
	mPass = EEPROM.readString(ADDR_PASS);
	mLocation = EEPROM.readString(ADDR_LOCATION);
  
  	mLocation = "운정";
	mSSID  = "SK_WiFiGIGA3506_2.4G";
	mPass = "BADB7@5046";
	mNetworkType = readLongFromEEPROM( ADDR_NETTYPE );     // 0 알수없음   1  무선랜   2 유선랜  3 lte
	mMode = read( ADDR_MODE);
	//mLte = read( ADDR_LTE );
	Serial.println("  >> net type : " + String(mNetworkType) );
	mEthConnected = false;
}

void CMyEEPROM::writeEEPROM ( int addr, String p)
{
  EEPROM.writeString(addr, p); //EEPROM 주소 0 부터 ssid 기록
  EEPROM.commit(); //기록한 내용을 저장


 switch ( addr )
 {
 	case ADDR_LOCATION:  	mLocation = EEPROM.readString( addr ); break;
	case ADDR_OWNERID:		mOwnerID = EEPROM.readString( addr ); break;
	case ADDR_SSID 	 :		mSSID = EEPROM.readString( addr ); break;
	case ADDR_PASS		:mPass = EEPROM.readString( addr ); break;
	default : Serial.println("out of address...");
 }
}


int CMyEEPROM::writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
	byte len = strToWrite.length();

	//PRINT( F("Write length"), len );
	EEPROM.write(addrOffset, len);
	for (int i = 0; i < len; i++)
	{
		EEPROM.write(addrOffset + 1 + i, strToWrite[i]);

		//Serial.println( strToWrite[i]);
	}

	EEPROM.commit(); //기록한 내용을 저장
	return (addrOffset + 1 + len);
}

int CMyEEPROM::readStringFromEEPROM(int addrOffset, String *strToRead)
{
	int newStrLen = EEPROM.read(addrOffset);      // read length

	//Serial.println( "readStringFromEEPROM :" + newStrLen);
	if ( newStrLen > 20)
	{
		Serial.println("ERR: read over addr : " + String(addrOffset)+ ", len :" +  String(newStrLen) );
		return 0;
	}
	char data[newStrLen + 1];
	for (int i = 0; i < newStrLen; i++)
	{
		data[i] = EEPROM.read(addrOffset + 1 + i);
    	//Serial.println( ">>" + data[i]);
	}
	data[newStrLen] = '\0'; // !!! NOTE !!! 
	                        // Remove the space between the slash "/" and "0" 
							// (I've added a space because otherwise there is a display bug)
	*strToRead = String(data);
	return (addrOffset + 1 + newStrLen);
}

/** 
 *  
 */
bool CMyEEPROM::isDhcpPref()
{
    //int n =  readStringFromEEPROM(ADDR_DHCP, &dhcp);
    //return (dhcp.equals(F("true")));
	return (  EEPROM.read(ADDR_DHCP) );
}

bool CMyEEPROM::isLog()
{
    //  String dhcp;
    //int n =  readStringFromEEPROM(ADDR_LOG, &dhcp);
    //return (dhcp.equals(F("true")));
	return (  EEPROM.read(ADDR_LOG) );
}

//long readLongFromEEPROM(int addrOffset )
//{
//  String val;
//  readStringFromEEPROM(addrOffset, &val);
//   return  atol(val.c_str()); 
//}

int CMyEEPROM::writeLongToEEPROM(int addrOffset, long val )
{

    switch ( addrOffset )
    {
        case ADDR_NETTYPE: mNetworkType = val ;
        
    }

    
  String strVal(val);
  return (writeStringToEEPROM(addrOffset, strVal));
}

long CMyEEPROM::readLongFromEEPROM( int addrOffset) 
{
	//Serial.println( "read" + String(addrOffset) );
	String strVal;
	readStringFromEEPROM( addrOffset, &strVal );
	return atol(strVal.c_str());
}


void CMyEEPROM::put(int addrOffset, int  val )
{
	Serial.println("write add " + String(addrOffset) );
	
  	EEPROM.write(addrOffset, (byte) val);
  	EEPROM.commit(); //기록한 내용을 저장

	switch ( addrOffset )
	{
		case ADDR_MODE:	mMode = val; break;
		//case ADDR_SCREEN_TYPE : mScreenType = val; break;
		//case ADDR_SCREEN_ROT  : mScreenRot = val; break;
		//case ADDR_LTE : mLte = val; break;
	}
}

byte CMyEEPROM::read( int addrOffset) 
{
	return EEPROM.read(addrOffset);
}
