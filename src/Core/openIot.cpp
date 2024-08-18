#include "openIot.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

/**
 * Global Objects  
 **/ 
Preferences prefs;
AsyncWebSocket ws("/ws");
AsyncWebServer webServer(80);
ElementManager elementManager;
IotCore iotCore;
Syslog syslog;
Ntp ntp;
Mqtt mqtt;
WebUpdate webUpdate;
QueueHandle_t taskQueue;

/**
 * The core pages and web elements
 */
HotspotForm hotspotForm;
WifiForm wifiForm;
MqttForm mqttForm;
NtpForm ntpForm;
DeviceForm deviceForm;
LoggingForm loggingForm;
UpdateServerForm updateServerForm;
RebootForm rebootForm;
CountdownDialog rebootCountdown ("_SYS_REBOOT_COUNTDOWN");

/**
 * Reboot the device
 */
FormLabel rebootNeeded ("_SYS_REBOOT_NEEDED", []() {
  iotCore.doReboot();
});

void IotCore::doReboot () {
  rebootCountdown.start();
  iotCore.logIPrintln(F("Reboot request received"));
  delay (2000);
  ESP.restart();
}

/**
* The Accesspoint is started if a Wifi Network cannot be reached. 
* 
* The device will wait for 5 Minutes for configuration and the restart. 
* The reason for this is in case of an power outage the Accesspoints and the rest 
* of the network may not be avaliable when the device is rebooted. This would cause the device to stay in AP mode.
 */
Ticker accessPointModeTicker;

int resetCounter = 0;
void resetApMode() {
  iotCore.apWatchDog();
}

void IotCore::apWatchDog() {
  resetCounter ++;
  if (resetCounter == 300){
    iotCore.logDPrintln (F("Accesspoint mode timeout. -> Reset"));
    ESP.restart();
  }

  //--- Flash status LED to show we are in AP Mode ---
  digitalWrite(iotCore.statusLedPin, HIGH);   
  delay(100);                      
  digitalWrite(iotCore.statusLedPin, LOW);
}

//--- Umdate the core MQTT Data every 5 Seconds -----------
Ticker mqttStatusTicker;
void updateMqttStatus() {
  iotCore.updateMqttStatus();
}

void IotCore::updateMqttStatus() {
  String sTopicPrefix = mqttPrefix + "/sys/status";
  const char * topicPrefix = sTopicPrefix.c_str();

  char timebuffer[32];
  ntp.printLocalTime(timebuffer, 32, "%d.%m.%Y %H:%M:%S" );

  IPAddress ip = WiFi.localIP();
  char ipBuffer[20];
  sprintf (ipBuffer, "%u.%u.%u.%u ", ip[0], ip[1], ip[2], ip[3]);

  mqtt.publish (topicPrefix, "Application",(applicationName + "_b" + applicationBuild).c_str());
  mqtt.publish (topicPrefix, "IP-Address", ipBuffer);
  mqtt.publish (topicPrefix, "LastUpdate",timebuffer);
  mqtt.publish (topicPrefix, "Wifi-SSID", WiFi.SSID().c_str());
  mqtt.publish (topicPrefix, "Wifi-RSSI", (int)WiFi.RSSI());
  mqtt.publish (topicPrefix, "Heap-Memory", esp_get_free_heap_size());
}

IotCore::IotCore () : 
  netStatus(NET_NONE){
}

/**
 * This initializes the core system and creates the connections to the different servers.
 */
void IotCore::begin(int _statusLedPin, String _applicationName, int _applicationBuild) {
  statusLedPin     = _statusLedPin;
  applicationName  = _applicationName;
  applicationBuild = _applicationBuild;
  taskQueue = xQueueCreate(MAX_TASKS, sizeof(Task));

  readPreferences();
  pinMode(_statusLedPin, OUTPUT);
  digitalWrite (statusLedPin, HIGH);
  
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }

  /**
   * Connect to the nework or start an accesspoint as fallback
   */
  scanWifi();
  setupWifi();

  if (netStatus == NET_STA) {
    //--- Connected to a Wifi Network ----------------------------
    digitalWrite (statusLedPin, LOW);
    if (syslogServer != "") {
      syslog.begin (String(WiFi.getHostname()), _applicationName, applicationBuild,syslogServer.c_str()) ;
    }

    if (updateServerEnable && updateServer != "") {
      webUpdate.begin();
    }

    if (ntpEnable && (ntpServer1 != "" || ntpServer2 != "")) {
      ntp.begin (ntpOffsetGmt, ntpOffsetDst, ntpServer1.c_str(), ntpServer2.c_str());
    }

    if (mqttEnable && mqttServer != "" && mqttPort != 0 && mqttPrefix != "") {
      mqtt.begin (mqttServer.c_str(),mqttPort, mqttClientId.c_str(), mqttUserName.c_str(), mqttPassword.c_str(), mqttPrefix.c_str());
      mqttStatusTicker.attach (5,::updateMqttStatus);
      updateMqttStatus();
    } 
  } else {
    //--- Access point --------------------------------
    accessPointModeTicker.attach(1,::resetApMode);
  }

  //--- Initialize websockets ----------------
  ws.onEvent(onWebSocketEvent);

  //--- init the flash filesystem -------------------
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //--- initalize webserver components ------------------
  logPreferences();
  elementManager.initRest ();
  webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  webServer.addHandler(&ws);
  webServer.begin();
  logDPrintln(F("HTTP server started"));

  //--- Initialize the core web pages --------------------
  hotspotForm.begin();
  wifiForm.begin();
  mqttForm.begin();
  ntpForm.begin();
  deviceForm.begin();
  loggingForm.begin();
  updateServerForm.begin();
  rebootForm.begin();
}

/**
 * Loop method needs to be called from the main loop method
 */
void IotCore::loop () {
  ws.cleanupClients();
  mqtt.loop();
  Task task;
  if (xQueueReceive(taskQueue, &task, 0) == pdPASS) {
    task.function(task.parameter);
  }  
}

/**
 * Logging Methods
 */
void IotCore::logDPrintln (const __FlashStringHelper* format, ...) {
  va_list args;
  va_start(args, format);
  logMessageInt (0,format, args);
}

void IotCore::logIPrintln (const __FlashStringHelper* format, ...) {
  va_list args;
  va_start(args, format);
  logMessageInt (1,format, args);
}

void IotCore::logWPrintln (const __FlashStringHelper* format, ...) {
  va_list args;
  va_start(args, format);
  logMessageInt (2,format, args);
}

void IotCore::logEPrintln (const __FlashStringHelper* format, ...) {
  va_list args;
  va_start(args, format);
  logMessageInt (3,format, args);
}

void IotCore::logMessageInt (int level, const __FlashStringHelper* format, va_list args) {
  char message[256];
	vsnprintf(message, 256, (const char*)format, args);
	va_end(args);

  if (syslogEnable && syslogServer != "" && netStatus == NET_STA)
  {
    switch (level)  {
      case 0 : syslog.print(FAC_LOCAL7, PRI_DEBUG,   message); break;
      case 1 : syslog.print(FAC_LOCAL7, PRI_INFO,    message); break;
      case 2 : syslog.print(FAC_LOCAL7, PRI_WARNING, message); break;
      case 3 : syslog.print(FAC_LOCAL7, PRI_ERROR,   message); break;
    }
  } else {
    switch (level)
    {
      case 0 : Serial.print ("DEBUG - "); break;
      case 1 : Serial.print ("INFO  - "); break;
      case 2 : Serial.print ("WARN  - "); break;
      case 3 : Serial.print ("ERR   - "); break;
    }
    Serial.println (message);
  }
}

/**
 * Read and write preferences 
 */
void IotCore::readPreferences () {
  char lHostname[23];
  uint64_t chipid = ESP.getEfuseMac();
  uint16_t chip = (uint16_t)(chipid >> 32);
  snprintf(lHostname, 23, "OPEN-IOT-%04X%08X", chip, (uint32_t)chipid);

  prefs.begin ("IotCore");
  ap_ssid        = prefs.getString ("ap_ssid", lHostname);
  ap_password    = prefs.getString ("ap_password", "");
  ap_open        = prefs.getBool("ap_open", true);

  wifi_enable_primary = prefs.getBool("wifi_en_prim");
  wifi_ssid1     = prefs.getString ("wifi_ssid1", "");
  wifi_password1 = prefs.getString ("wifi_password1", "");
  wifi_enable_secondary = prefs.getBool("wifi_en_sec");
  wifi_ssid2     = prefs.getString ("wifi_ssid2", "");
  wifi_password2 = prefs.getString ("wifi_password2", "");

  mqttEnable      = prefs.getBool ("mqttEnable",false);
  mqttServer      = prefs.getString ("mqttServer","");
  mqttPort        = prefs.getInt ("mqttPort", 1883);
  mqttClientId    = prefs.getString ("mqttClientId","");
  mqttUserName    = prefs.getString ("mqttUserName","");
  mqttPassword    = prefs.getString ("mqttPassword","");
  mqttPrefix      = prefs.getString ("mqttPrefix",hostname);

  ntpEnable      = prefs.getBool ("ntpEnable",false);
  ntpServer1     = prefs.getString ("ntpServer1", "time.google.com");
  ntpServer2     = prefs.getString ("ntpServer2", "time.google.com");
  ntpOffsetGmt   = prefs.getInt ("ntpOffsetGmt", 3600);
  ntpOffsetDst   = prefs.getInt ("ntpOffsetDst", 3600);

  syslogEnable      = prefs.getBool("syslogEnable", false);
  syslogServer      = prefs.getString ("syslogServer", "");

  hostname       = prefs.getString ("hostname", lHostname);

  updateServerEnable = prefs.getBool ("updateSrvEn", false);
  updateServer       = prefs.getString ("updateServer","");
  autoUpdate         = prefs.getBool ("autoUpdate", false);

  prefs.end();
}

void IotCore::writePreferences (bool isRebootNeeded) {
  if (checkPreferencesChanged()) {
    prefs.begin ("IotCore");
    prefs.putString ("ap_password",    ap_password);
    prefs.putString ("ap_ssid",        ap_ssid);
    prefs.putBool   ("ap_open",        ap_open);

    prefs.putBool   ("wifi_en_prim",   wifi_enable_primary);
    prefs.putString ("wifi_ssid1",     wifi_ssid1);
    prefs.putString ("wifi_password1", wifi_password1);
    prefs.putBool   ("wifi_en_sec",    wifi_enable_secondary);
    prefs.putString ("wifi_ssid2",     wifi_ssid2);
    prefs.putString ("wifi_password2", wifi_password2);

    prefs.putBool   ("mqttEnable",     mqttEnable);
    prefs.putString ("mqttServer",     mqttServer);
    prefs.putInt    ("mqttPort",       mqttPort);
    prefs.putString ("mqttClientId",   mqttClientId);
    prefs.putString ("mqttUserName",   mqttUserName);
    prefs.putString ("mqttPassword",   mqttPassword);
    prefs.putString ("mqttPrefix",     mqttPrefix);

    prefs.putBool   ("ntpEnable",      ntpEnable);
    prefs.putString ("ntpServer1",     ntpServer1);
    prefs.putString ("ntpServer2",     ntpServer2);
    prefs.putInt    ("ntpOffsetGmt",   ntpOffsetGmt);
    prefs.putInt    ( "ntpOffsetDst",  ntpOffsetDst);

    prefs.putString ("hostname",       hostname);

    prefs.putBool   ("syslogEnable",   syslogEnable);
    prefs.putString ("syslogServer",   syslogServer);

    prefs.putBool   ("updateSrvEn",    updateServerEnable);
    prefs.putString ("updateServer",   updateServer);
    prefs.putBool   ("autoUpdate",     autoUpdate);

    if (isRebootNeeded) {
      logIPrintln (F("Preferences changed and reboot is needed"));
      rebootNeeded.setValue F("Reboot is needed. Click here to reboot !");
    } else {
      logIPrintln (F("Preferences changed. No reboot neccessary."));
    }
    prefs.end();
  }
}

boolean IotCore::checkPreferencesChanged () {
  if (checkPreferenceChanged ("ap_password",    ap_password))           return true; 
  if (checkPreferenceChanged ("ap_ssid",        ap_ssid))               return true; 
  if (checkPreferenceChanged ("ap_open",        ap_open))               return true; 

  if (checkPreferenceChanged ("wifi_en_prim",   wifi_enable_primary))   return true; 
  if (checkPreferenceChanged ("wifi_ssid1",     wifi_ssid1))            return true; 
  if (checkPreferenceChanged ("wifi_en_sec",    wifi_enable_secondary)) return true; 
  if (checkPreferenceChanged ("wifi_password1", wifi_password1))        return true; 
  if (checkPreferenceChanged ("wifi_ssid2",     wifi_ssid2))            return true; 
  if (checkPreferenceChanged ("wifi_password2", wifi_password2))        return true; 

  if (checkPreferenceChanged ("mqttEnable",     mqttEnable))            return true; 
  if (checkPreferenceChanged ("mqttServer",     mqttServer))            return true; 
  if (checkPreferenceChanged ("mqttPort",       mqttPort))              return true; 
  if (checkPreferenceChanged ("mqttClientId",   mqttClientId))          return true; 
  if (checkPreferenceChanged ("mqttUserName",   mqttUserName))          return true; 
  if (checkPreferenceChanged ("mqttPassword",   mqttPassword))          return true; 
  if (checkPreferenceChanged ("mqttPrefix",     mqttPrefix))            return true; 

  if (checkPreferenceChanged ("ntpEnable",      ntpEnable))             return true; 
  if (checkPreferenceChanged ("ntpServer1",     ntpServer1))            return true; 
  if (checkPreferenceChanged ("ntpServer2",     ntpServer2))            return true; 
  if (checkPreferenceChanged ("ntpOffsetGmt",   ntpOffsetGmt))          return true; 
  if (checkPreferenceChanged ("ntpOffsetDst",   ntpOffsetDst))          return true; 

  if (checkPreferenceChanged ("syslogEnable",   syslogEnable))          return true; 
  if (checkPreferenceChanged ("syslogServer",   syslogServer))          return true; 

  if (checkPreferenceChanged ("hostname",       hostname))              return true; 

  if (checkPreferenceChanged ("updateSrvEn",    updateServerEnable))    return true; 
  if (checkPreferenceChanged ("updateServer",   updateServer))          return true; 
  if (checkPreferenceChanged ("autoUpdate",     autoUpdate))            return true; 

  return false;
}

boolean IotCore::checkPreferenceChanged (String key, String newValue) {
  prefs.begin ("IotCore");
  String oldValue = prefs.getString (key.c_str(), "");
  if (oldValue == newValue) { 
    return false;
  }
  logDPrintln (F("Key %s changed old : '%s' new : '%s'"),key.c_str(), oldValue.c_str(), newValue.c_str());
  prefs.end();
  return true;
}

boolean IotCore::checkPreferenceChanged (String key, int32_t newValue) {
  prefs.begin ("IotCore");
  int32_t oldValue = prefs.getInt (key.c_str(), 0);
  if (oldValue == newValue) { 
    return false;
  }
  logDPrintln (F("Key %s changed old : '%d' new : '%d'"),key.c_str(), oldValue, newValue);
  prefs.end();
  return true;
}

boolean IotCore::checkPreferenceChanged (String key, bool newValue) {
  prefs.begin ("IotCore");
  bool oldValue = prefs.getBool (key.c_str(), false);
  if (oldValue == newValue) { 
    return false;
  }
  logDPrintln (F("Key %s changed old : '%s' new : '%s'"),key.c_str(), (oldValue ? "true" : "false"), (newValue ? "true" : "false"));
  prefs.end();
  return true;
}


void IotCore::logPreferences () {
  logDPrintln (F("----------------------------------------------------------------------"));
  logDPrintln (F("Application started   : %s build %d "),applicationName.c_str(),applicationBuild);
  logDPrintln (F("Accesspoint SSID      : %s"), ap_ssid.c_str());
  logDPrintln (F("Accesspoint Password  : %s"), ap_password.c_str());
  logDPrintln (F("Accesspoint Open      : %s"), (ap_open ? "true" : "false"));

  logDPrintln (F("Wifi Primary Enabled  : %s"), (wifi_enable_primary ? "true" : "false"));
  logDPrintln (F("Primary ssid          : %s"), wifi_ssid1.c_str());
  logDPrintln (F("Primary Password      : %s"), wifi_password1.c_str());
  logDPrintln (F("Wifi Seconday Enabled : %s"), (wifi_enable_secondary ? "true" : "false"));
  logDPrintln (F("Secondary ssid        : %s"), wifi_ssid2.c_str());
  logDPrintln (F("Secondary Password    : %s"), wifi_password2.c_str());

  logDPrintln (F("Mqtt enabled          : %s"), (mqttEnable ? "true" : "false"));
  logDPrintln (F("Mqtt Server           : %s:%d"),mqttServer.c_str(),mqttPort);
  logDPrintln (F("Mqtt Client Id        : %s"), mqttClientId.c_str());
  logDPrintln (F("Mqtt UsernName        : %s"), mqttUserName.c_str());
  logDPrintln (F("Mqtt Password         : %s"), mqttPassword.c_str());
  logDPrintln (F("Mqtt Prefix           : %s"), mqttPrefix.c_str());

  logDPrintln (F("Ntp enabled           : %s"), (ntpEnable ? "true" : "false"));
  logDPrintln (F("Ntpserver1            : %s"), ntpServer1.c_str());
  logDPrintln (F("Ntpserver2            : %s"), ntpServer2.c_str());
  logDPrintln (F("Ntp Offset Gmt        : %d"), ntpOffsetGmt);
  logDPrintln (F("Ntp Offset Dst        : %d"), ntpOffsetDst);

  logDPrintln (F("Syslog enabled        : %s"), (syslogEnable ? "true" : "false"));
  logDPrintln (F("Syslogserver          : %s"), syslogServer.c_str());

  logDPrintln (F("Hostname              : %s"), hostname.c_str());
 
  logDPrintln (F("UpdateServer Enabled  : %s"), (updateServerEnable ? "true" : "false"));
  logDPrintln (F("UpdateServer          : %s"), updateServer.c_str());
  logDPrintln (F("AutoUpdate            : %s"), (autoUpdate ? "true" : "false"));
  
  if (netStatus == NET_STA) {
    logDPrintln (F("Network               : Station Mode"));
  } else {
    logDPrintln (F("Network               : Accesspoint"));
  }
  logDPrintln (F("SSID                  : %s "), WiFi.SSID().c_str());
  logDPrintln (F("RSSI                  : %d "), WiFi.RSSI());
  logDPrintln (F("Channel               : %d "),  WiFi.channel());
  if (netStatus == NET_STA) {
    IPAddress ip = WiFi.localIP();
    logDPrintln (F("IP address            : %u.%u.%u.%u "), ip[0], ip[1], ip[2], ip[3]);
  } else {
    IPAddress ip = WiFi.softAPIP();
    logDPrintln (F("IP address            : %u.%u.%u.%u "), ip[0], ip[1], ip[2], ip[3]);
  }
  logDPrintln (F("----------------------------------------------------------------------"));
}

/**
 * Scan for avalable Wifi Networks
 * The scan result will be stored to a local map for usage in the web interface
 */
void IotCore::scanWifi () {
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    int32_t rssi = WiFi.RSSI(i);

    // Filter out duplicate SSID's and save the one with the highest rssi
    if (networkMap.find(ssid) == networkMap.end() || networkMap[ssid].rssi < rssi) {
      networkMap[ssid].ssid = ssid;
      networkMap[ssid].description = ssid + " (" + String(rssi) + " dBm)";
    }
  }
}  

/**
 * Setup the Wifi Network
 * If both Wifi networks are enabled, the device will connect to the strongest signal. 
 */ 
void IotCore::setupWifi () {
  logDPrintln (F("Set up wifi connection"));

  if (hostname != "") {
     WiFi.setHostname (hostname.c_str()); 
  } else {
    //--- Create a local hostname from Mac Adress -------
    char lHostname[23];
    uint64_t chipid = ESP.getEfuseMac();
    uint16_t chip = (uint16_t)(chipid >> 32);
    snprintf(lHostname, 23, "OPEN-IOT-%04X%08X", chip, (uint32_t)chipid);
     WiFi.setHostname (lHostname); 
  }
  
  String bestSSID = "";
  String bestPassword = "";
  unsigned long startAttemptTime = millis();
  /**
   * If both ssid's are enabled and could be reached, select the stronger one. 
   */
  if (wifi_enable_primary && 
    networkMap.find(wifi_ssid1) != networkMap.end()  &&  
    wifi_enable_secondary &&
    networkMap.find(wifi_ssid2) != networkMap.end() ) {
    if (networkMap[wifi_ssid1].rssi > networkMap[wifi_ssid2].rssi) {
      bestSSID = wifi_ssid1;
      bestPassword = wifi_password1;
    } else {
      bestSSID = wifi_ssid2;
      bestPassword = wifi_password2;
    }
  } else if (wifi_enable_primary && networkMap.find(wifi_ssid1) != networkMap.end()) {
    bestSSID = wifi_ssid1;
    bestPassword = wifi_password1;
  } else if (wifi_enable_secondary && networkMap.find(wifi_ssid2) != networkMap.end()) {
    bestSSID = wifi_ssid2;
    bestPassword = wifi_password2;
  }

 if (bestSSID != "") {
    //--- Connect to the selected network -------------------------
    logDPrintln (F("Connect to %s"), bestSSID.c_str());

    WiFi.begin(bestSSID.c_str(), bestPassword.c_str());
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
      logDPrintln (F("Connected to %s"), bestSSID.c_str());
      netStatus = NET_STA;
    } else {
      logDPrintln (F("\nFailed to connect to %s"), bestSSID.c_str());
      startAccessPoint();
    }
  } else {
    logDPrintln (F("None of the preferred SSIDs are available."));
    startAccessPoint();
  }
}

/**
 * Start the fallback accesspoint
 */
void IotCore::startAccessPoint () {
  logDPrintln (F("No network found ! Starting Fallback AP "));
  if (!WiFi.softAP(ap_ssid,ap_password)) {
    logEPrintln (F("Soft AP creation failed."));
    ESP.restart();
  } 
  IPAddress ip = WiFi.softAPIP();
  logIPrintln (F("Accesspoint ready. IP address:  %u.%u.%u.%u "), ip[0], ip[1], ip[2], ip[3]);
  netStatus = NET_AP;
}
