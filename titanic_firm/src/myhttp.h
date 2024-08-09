#pragma once



#include <ArduinoJson.h>
#include <HTTPClient.h>            // HTTP GET ( esp32용 쓸거. )

class CMyHttp
{
	public:
	CMyHttp();
	~CMyHttp();

public:
	void downloadAndSaveFileSPIFFS(String fileName, String  url);

//private:
	void patch_datetime();
	void patch_finedust_();

  static bool parse_finedust_( const char * pjson );

  void fetch();



public:
    void begin(  WiFiClientSecure         *pespClient)
    {
        mpespClient = pespClient;
    }

public:
    String  http_get_json( const char *url_api );
    static String urlEncode(const char *msg);
   

private :

    String          payload;
    HTTPClient      https;
    WiFiClientSecure *mpespClient;



};


