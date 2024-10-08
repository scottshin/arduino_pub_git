#include <arduino.h>
#include <esp_sleep.h>
#include "esp_util.h"

touch_pad_t touchPin;


void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

int print_wakeup_touchpad(){
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println( F("Touch detected on GPIO 4") ); break;
    case 1  : Serial.println( F("Touch detected on GPIO 0") ); break;
    case 2  : Serial.println( F("Touch detected on GPIO 2") ); break;
    case 3  : Serial.println( F("Touch detected on GPIO 15") ); break;
    case 4  : Serial.println( F("Touch detected on GPIO 13") ); break;
    case 5  : Serial.println( F("Touch detected on GPIO 12") ); break;
    case 6  : Serial.println( F("Touch detected on GPIO 14") ); break;
    case 7  : Serial.println( F("Touch detected on GPIO 27") ); break;
    case 8  : Serial.println( F("Touch detected on GPIO 33") ); break;
    case 9  : Serial.println( F("Touch detected on GPIO 32") ); break;
    default : Serial.println( F("Wakeup not by touchpad") ); 
              return -1;  
  }

  return (touchPin);
}

void esp_goto_deep_sleep_now()
{ 
    Serial.println( F("Going to sleep now") );
    delay(1000);
    Serial.flush(); 
    esp_deep_sleep_start();
    Serial.println( F("This will never be printed") );
}

void esp_restart_now()
{
    //esp_restart();
        esp_sleep_enable_timer_wakeup(1000000 * 3);
        esp_deep_sleep_start();
}
