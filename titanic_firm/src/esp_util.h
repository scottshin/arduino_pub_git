
#pragma once 

#include "variable.h"

#include <arduino.h>
#include <esp_sleep.h>
#include "EEPROM.h"

void print_wakeup_reason();
int print_wakeup_touchpad();

void esp_goto_deep_sleep_now();

void esp_restart_now();
