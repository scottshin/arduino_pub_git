#pragma once

#include <Arduino.h>


class CMyEEPROM
{
public:
	CMyEEPROM();
	~CMyEEPROM();

	void init();
  
	int writeStringToEEPROM(int addrOffset, const String &strToWrite);
	int readStringFromEEPROM(int addrOffset, String *strToRead);
	//long readLongFromEEPROM(int addrOffset );
	int writeLongToEEPROM(int addrOffset, long val );
	long readLongFromEEPROM( int addrOffset) ;

	bool isDhcpPref();
	void parse_command(char *cmd);

	bool isLog();
	void writeEEPROM( int addr, String p);

	void put(int addrOffset, int  val );
	byte read( int addrOffset) ;

public:
	int mMode;			// ble, wifi

      String mLocation;
      String mThingName;
      String mOwnerID;

      String mSSID;
      String mPass;
      
    int mNetworkType;


    char szDate[0x10];
    char szTime[0x10];

    int		mStatus;      // 0 is idle   1 is running

	int		carspeed;
    int		pm25;
    int		pm10;
    int		weather;			// sky + pty 
    int		humidity;
    int		temp;
    //String schedule_json;

    String	traffic_rss;
    String	forecast[40]; 
    String	mRoadName;


    bool mEthConnected;

};
