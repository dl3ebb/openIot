#include "openIot.h"
/**
 * Logs to a central syslog server. The messages are sent asynchrounous to the server,
 * so response time will not be affected by the syslog traffic. 
 */

Ticker messageTicker;
WiFiUDP logServerUdp;

/**
 * callback for the send ticker. 
 */
void sendLogs() {
  syslog.sendMessages();
}

void Syslog::sendMessages() {
  String message;
  int mSeverity = -1;
  portENTER_CRITICAL(&queueMux);
    if (queueCount > 0)  {
      message = messageQueue[queueFront];
      mSeverity = messageSeverity[queueFront];
      queueFront = (queueFront + 1) % QUEUE_SIZE;
      queueCount--;
    }
  portEXIT_CRITICAL(&queueMux);

  if (mSeverity != -1)  {
    String syslogMsg = "<" + String(mSeverity) + ">" + hostname + " " + appName + "_b" + String(appVersion) + ": " + message;
    logServerUdp.beginPacket(logServer, port);
    logServerUdp.print(syslogMsg);
    logServerUdp.endPacket();
  }
}


void Syslog::begin(String _hostname, String _appName, int _appVersion, const char* _logServer, uint16_t _port) {
  this->logServer  = _logServer;
  this->port       = _port;
  this->hostname   = _hostname + "-" +  WiFi.localIP().toString();
  this->appName    = _appName;
  this->appVersion = _appVersion;
  messageTicker.attach(0.03, ::sendLogs);
}

/**
 * the print method will just queue the message. 
 */
void Syslog::print(uint8_t facility, uint8_t severity, char *mesg) {
  // The PRI value is an integer number which calculates by the following metric:
  uint8_t priority = (8 * facility) + severity;
  String syslogMsg = String(mesg);

  //--- In die Queue legen ------------------------------------
  portENTER_CRITICAL(&queueMux);
  if (queueCount < QUEUE_SIZE)
  {
    messageQueue[queueBack] = syslogMsg;
    messageSeverity[queueBack] = priority;
    queueBack = (queueBack + 1) % QUEUE_SIZE;
    queueCount++;
  }
  portEXIT_CRITICAL(&queueMux);
}
