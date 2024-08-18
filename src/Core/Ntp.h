#include "openIot.h"
#ifndef NTP_H
#define NTP_H

class Ntp {
	
	public:
		void begin (long gmtOffsetSec, long dstOffsetSec);
		void begin (long gmtOffsetSec, long dstOffsetSec, const char* _ntpServer1, const char* _ntpServer2);
		void printLocalTime(char *buf, int maxlen, const char* format);

};
#endif