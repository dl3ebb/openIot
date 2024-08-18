#ifndef OpenIot_h
#define OpenIot_h

#include "Arduino.h"

#include <Preferences.h>
#include <Ticker.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include "time.h"
#include "esp_sntp.h"
#include <strings.h>
#include <map>


#include <Core/WebSocketHandler.h>
#include <Core/Syslog.h>
#include <Core/Ntp.h>
#include <Core/Mqtt.h>
#include <Core/WebUpdate.h>


extern Preferences prefs;
extern WiFiUDP logServerUdp;
extern AsyncWebSocket ws;
extern AsyncWebServer webServer;
class ElementManager;
extern ElementManager elementManager;
extern Syslog syslog; 
extern Ntp ntp; 
extern Mqtt mqtt;
extern WebUpdate webUpdate;

#define NET_NONE 0
#define NET_AP   1
#define NET_STA  2


struct WifiNetwork {
  String ssid;
  int32_t rssi;
  String description;
};

#define MAX_TASKS 10

/**
 * Background Tasks 
 */
struct Task {
    void (*function)(void *); 
    void *parameter;          
};

// Queue for Background tasks 
extern QueueHandle_t taskQueue;


class IotCore {
  public:
    IotCore() ;
    void begin(int _statusLedPin, String _applicationName, int _applicationBuild);

    void logDPrintln (const __FlashStringHelper* format, ...);
    void logIPrintln (const __FlashStringHelper* format, ...);
    void logWPrintln (const __FlashStringHelper* format, ...);
    void logEPrintln (const __FlashStringHelper* format, ...);

    int getNetStatus () { 
      return netStatus;
    }
    
    void loop ();
    void apWatchDog();
    void updateMqttStatus();
    void scanWifi ();
    void doReboot ();

    //--- Accesspoint -----------------------------------
    String ap_password;
    String ap_ssid;
    bool ap_open;

    //--- Wifi ---------------------------------------
    bool wifi_enable_primary;
    String wifi_ssid1;
    String wifi_password1;
    bool wifi_enable_secondary;
    String wifi_ssid2;
    String wifi_password2;

    //--- MQTT ------------------------------------------
    bool mqttEnable;
    String mqttServer;
    int mqttPort;
    String mqttClientId;
    String mqttUserName;
    String mqttPassword;
    String mqttPrefix;

    //--- Device Name -----------------------------------
    String hostname;

    //--- NTP -------------------------------------------
    bool ntpEnable;
    String ntpServer1;
    String ntpServer2;
    int ntpOffsetGmt;
    int ntpOffsetDst;
    
    //--- Syslog Server ---------------------------------
    bool syslogEnable;
    String syslogServer;  // config

    //--- Update ----------------------------------------
    bool updateServerEnable;
    String updateServer;
    bool autoUpdate;

    void writePreferences(bool rebootNeeded);

    //-- Resultat des letzten Netzwerkscans ---------------
    std::map<String, WifiNetwork> networkMap;

    String applicationName;
    int applicationBuild;


    int netStatus;

  private:
    int statusLedPin;
    int statusLedPinValue;



    //--- Logging -------------------------------------
    void logMessageInt (int level, const __FlashStringHelper* format, va_list args);

    //--- Read / Write Preferences ---------------------
    void readPreferences();
    boolean checkPreferencesChanged();
    boolean checkPreferenceChanged(String key, String newValue);
    boolean checkPreferenceChanged (String key, int32_t newValue); 
    boolean checkPreferenceChanged (String key, bool newValue);
    void logPreferences();

    //--- Wifi -----------------------------------------
    void setupWifi();
    void startAccessPoint ();
};

extern IotCore iotCore;

#include <Core/Element.h>
#include <Core/ElementManager.h>

#include <WebWidgets/Form.h>
#include <WebWidgets/LedButton.h>
#include <WebWidgets/FormCheckbox.h>
#include <WebWidgets/FormSubmitButton.h>
#include <WebWidgets/FormButton.h>
#include <WebWidgets/FormTextInput.h>
#include <WebWidgets/FormIntInput.h>
#include <WebWidgets/FormLabel.h>
#include <WebWidgets/FormSelectbox.h>
#include <WebWidgets/CountdownDialog.h>
#include <WebWidgets/ProgressDialog.h>

#include <WebUi/HotspotForm.h>
#include <WebUi/WifiForm.h>
#include <WebUi/MqttForm.h>
#include <WebUi/NtpForm.h>
#include <WebUi/DeviceForm.h>
#include <WebUi/LoggingForm.h>
#include <WebUi/UpdateServerForm.h>
#include <WebUi/RebootForm.h>

#endif
