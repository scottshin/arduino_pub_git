#pragma once

#include <BLEDevice.h>
//#include <BLE2902.h>
//#include <BLEUtils.h>
//#include <BLEServer.h>

#define SERVICE_UUID             "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX   "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
extern BLECharacteristic *pCharacteristic;
class CBleTask
{
public:
	CBleTask();
	~CBleTask();

	void start();
	void loop();

private:
	void display_statusbar();


public :
	static bool deviceConnected;

private:
	int pre_timer;
	int wait_time; 
};


