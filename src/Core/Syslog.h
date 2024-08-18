#include "openIot.h"
#ifndef SYSLOG_H
#define SYSLOG_H

#define PRI_EMERGENCY 0
#define PRI_ALERT     1
#define PRI_CRITICAL  2
#define PRI_ERROR     3
#define PRI_WARNING   4
#define PRI_NOTICE    5
#define PRI_INFO      6
#define PRI_DEBUG     7

#define FAC_USER   1
#define FAC_LOCAL0 16
#define FAC_LOCAL1 17
#define FAC_LOCAL2 18
#define FAC_LOCAL3 19
#define FAC_LOCAL4 20
#define FAC_LOCAL5 21
#define FAC_LOCAL6 22
#define FAC_LOCAL7 23

#define QUEUE_SIZE 30

class Syslog {
	private: 
		//--- Config ---------------------
		const char* logServer;  // syslogserver
		uint16_t    port;      // Port 

		String hostname;    // Eigener Hostname 
		String appName;     // Applikationsname 
		int    appVersion;  // Buildversion

        //--- Queues für die Messages --------
		String messageQueue[QUEUE_SIZE];   // Die Nachrichten
		int messageSeverity[QUEUE_SIZE];   // Severity der Nachrichten

		volatile int queueFront = 0;
		volatile int queueBack = 0;
		volatile int queueCount = 0;
		portMUX_TYPE queueMux = portMUX_INITIALIZER_UNLOCKED;

	public:
    	void sendMessages();    
    	void print(uint8_t facility, uint8_t severity, char *mesg);    
		void begin(String _hostname, String _appName, int _appVersion, const char* logServer, uint16_t _port =514);
};
#endif
