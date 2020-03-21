#include "Clock.h"

RTC_DS1307* Clock::rtc = NULL;
char Clock::current_time[5];