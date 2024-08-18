#include "openIot.h"
/**
 * Set up a connection to a NTP server
 */

#define sntpUpdateInterval 15L * 60 * 1000

#define NTP_SERVER_1 "pool.ntp.org"

const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval* t) {
  char timebuffer[32];
  ntp.printLocalTime(timebuffer, 32, "%d.%m.%Y %H:%M:%S" );
  iotCore.logDPrintln(F("NTP update : %s"), timebuffer);
}

void Ntp::begin (long gmtOffsetSec, long dstOffsetSec) {
	begin(gmtOffset_sec, dstOffsetSec, NTP_SERVER_1, NTP_SERVER_1);
}

void Ntp::begin (long gmtOffsetSec, long dstOffsetSec, const char* _ntpServer1, const char* _ntpServer2) {
	sntp_set_sync_interval(sntpUpdateInterval);
  sntp_set_time_sync_notification_cb(timeavailable);
	configTime(gmtOffset_sec, daylightOffset_sec, _ntpServer1, _ntpServer2);
}

void Ntp::printLocalTime(char *buf, int maxlen, const char* format) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
	  strptime("01011970", "%d%m%Y", &timeinfo);
  }
  strftime (buf,maxlen, format,&timeinfo);
}
