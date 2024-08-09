
#include "variable.h"
#include "myeeprom.h"


#include "myhttp.h"
//#include "myjson.h"
#include "myeeprom.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>			// HTTP GET

//#include <TimeLib.h>			// Inclue "Time" lib  
#include "SPIFFS.h"


//#include "schedule.h"

extern CMyEEPROM  	    eeprom;;


const String url_time    PROGMEM =  "https://api.p2sg.link/api/p2sg/localtime";
const String url_finedust PROGMEM = "https://api.p2sg.link/api/p2sg/finedust?local=";


      
CMyHttp::CMyHttp()
{

	
}
CMyHttp::~CMyHttp()
{
	
}

void CMyHttp::patch_datetime()
{
	Serial.println("Patch: Date/Time");
	String json =  this->http_get_json( url_time.c_str() );
  //"{\"timezone\":\"Asia/Seoul\", \"time\":{ \"year\":2023,\"month\":2,\"day\":7,\"hour\":4,\"min\":45,\"sec\":50} }";

  // JSON 데이터의 최대 크기 설정
  const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(6) + 70;
  DynamicJsonDocument doc(bufferSize);
  DeserializationError error = deserializeJson(doc, json.c_str() );
  if (error) {
    Serial.print("JSON 파싱 오류: ");
    Serial.println(error.c_str());
    return;
  }
  // time 객체의 내부 값을 출력
  JsonObject vtime_ = doc["time"];

  int year = vtime_["year"];
  int month = vtime_["month"];
  int _day = vtime_["day"];

  int hour = vtime_["hour"];
  int min = vtime_["min"];
	sprintf(eeprom.szDate, "%d/%d/%d", year, month, _day );
  sprintf(eeprom.szTime, "%02d:%02d", hour, min );
                           
}

/**
 * 
 */
void CMyHttp::patch_finedust_()
{

    Serial.println(" TIME: Patch_finedust.............2 ");
    String strLoc = CMyHttp::urlEncode ( eeprom.mLocation.c_str() );
    String json2 = this->http_get_json( (url_finedust + strLoc ) .c_str()      );

    CMyHttp::parse_finedust_( json2.c_str() );
}

//static 
bool CMyHttp::parse_finedust_( const char * pjson )
{
    // JSON 데이터를 파싱할 버퍼의 크기 설정
  const size_t bufferSize = JSON_OBJECT_SIZE(127);
  StaticJsonDocument<bufferSize> doc;


 // JSON 데이터 파싱
  DeserializationError error = deserializeJson(doc, pjson);
  if (error) {
    Serial.print("JSON 파싱 오류: ");
    Serial.println(error.c_str());
    return false ;
  }

  eeprom.pm10     = doc[0]["pm10"];
  eeprom.pm25    = doc[0]["pm25"];

  int sky         = doc[0]["sky"];
  int pty         = doc[0]["pty"];
  eeprom.temp     = doc[0]["temp"];
                 
  eeprom.humidity = doc[0]["humidity"];

  if ( eeprom.humidity > 99 )
            eeprom.humidity = 99;
            
  //eeprom.mLocation = obj["site_name"].as<String>();
  // 하늘상태 sky : 맑음(1), 구름많음(3), 흐림(4)
  // 강수형태 pty : 없음(0), 비(1), 비/눈(2), 눈(3), 소나기(4)
  if ( pty )
            eeprom.weather = (pty);
  else
            eeprom.weather = (sky+4);

  return true;    
}



void CMyHttp:: downloadAndSaveFileSPIFFS(String fileName, String  url)
{
	HTTPClient http;

	Serial.println("[HTTP] begin...\n");
	Serial.println(fileName);
	Serial.println(url);
	http.begin(url);
	
	Serial.printf("[HTTP] GET...\n", url.c_str());
	// start connection and send HTTP header
	int httpCode = http.GET();
	if(httpCode > 0) 
	{
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);
		Serial.printf("[FILE] open file for writing %s\n", fileName.c_str());
      
		File file = SPIFFS.open(fileName, FILE_WRITE);
		// file found at server
		if(httpCode == HTTP_CODE_OK) 
		{

          // get lenght of document (is -1 when Server sends no Content-Length header)
          int len = http.getSize();

           Serial.printf("[FILE] file size %d\n", len );

          // create buffer for read
          uint8_t buff[128] = { 0 };

          // get tcp stream
          WiFiClient * stream = http.getStreamPtr();

			if ( !stream )
          Serial.printf("[FILE] stream ptr null" );

          // read all data from server
          while(http.connected() && (len > 0 || len == -1)) {
              // get available data size
              size_t size = stream->available();
              if(size) {
                  // read up to 128 byte
                  //Serial.printf("[FILE]  read %d\n", size);
                  
                  int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                  // write it to Serial
                  //Serial.write(buff, c);
                  file.write(buff, c);
                  if(len > 0) {
                      len -= c;
                  }
              }
              delay(1);
          }

          Serial.println();
          Serial.println("[HTTP] connection closed or file end.\n");
          Serial.println("[FILE] closing file\n");
          file.close();
          
      }
      
	}
	http.end();
}


void CMyHttp::fetch()
{
  patch_datetime();
  delay(500);
  patch_finedust_();
}








/**
 *
 */
String CMyHttp::http_get_json( const char *url_api )
{

  if(WiFi.status() != WL_CONNECTED)
  {
        Serial.println("Error : wifi is not connected");
        return "error";
  }
   
 // https.setInsecure();  // Use certs, but do not check their authenticity
    https.begin( *(mpespClient), url_api ); //Specify the URL
    for ( int i = 0; i < 3; i++)
    {
        if ( i != 0 )
            Serial.println("    Retry....");
        int httpCode = https.GET();                                        //Make the request 
        if (httpCode > 0) { //Check for the returning code
            payload = https.getString();
            Serial.println("    RET: >> " + String(httpCode));
            Serial.println("    Payload >> " + payload);
            
            https.end(); //Free the resources
            return (payload);
        }
        else {
            Serial.println("    Error on HTTP request : " + String(httpCode) );
            Serial.println("    Error URL : " + String(url_api) );
        }
    }
    https.end(); //Free the resources
    return ("");

}


  //static 
  String CMyHttp::urlEncode(const char *msg) 
    {
        const char *hex = "0123456789ABCDEF";
        static String encodedMsg;
        encodedMsg = "";
        while (*msg != '\0') {
          if (   ('a' <= *msg && *msg <= 'z') || 
                  ('A' <= *msg && *msg <= 'Z') || 
                 ('0' <= *msg && *msg <= '9') || 
                  *msg == '-' || *msg == '_' || *msg == '.' || *msg == '~') 
          {
                 encodedMsg += *msg;
          } else {
            encodedMsg += '%';
            encodedMsg += hex[(unsigned char)*msg >> 4];
            encodedMsg += hex[*msg & 0xf];
          }
          msg++;
        }
        return encodedMsg;
    }
