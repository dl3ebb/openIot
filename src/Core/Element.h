#include "openIot.h"
#ifndef APP_ELEMENT_H
#define APP_ELEMENT_H

/**
 * Element is the Baseclass for all Elements in the application. It consists of multiple interfaces for Websockets, MQTT, Rest ...
*/

extern AsyncWebSocket ws;

class Element {
  protected:
    String id;
    String error;

  public:
    Element(String _id) : id(_id), error("") {}
    virtual ~Element() {}
    virtual String getId() const { return id; }

    //--- Mqtt Interface -----------------------------------------------
    virtual bool isMqttElement() const { return false; }
    virtual void publishMqtt () { iotCore.logWPrintln (F("Element %s does not implement publishMqtt"), id.c_str()); }
    virtual void onMqttConnect () { iotCore.logWPrintln (F("Element %s does not implement onMqttConnect"), id.c_str()); }
    virtual void onMqttMessage (const char * topic, const char* value)  { iotCore.logWPrintln (F("Element %s does not implement onMqttMessage"), id.c_str()); }

    //--- Rest interface -----------------------------------------------
    virtual bool isRestElement() const { return false; }
    virtual void onRestRequest (const char * payload, AsyncWebServerRequest *request) { iotCore.logWPrintln (F("Element %s does not implement onRestReques"), id.c_str()); }

    //--- Web Interface ------------------------------------------------
    virtual bool isWebElement() const { return false; }
    virtual void handleWebEvent(int argc, char* argv[]) { iotCore.logWPrintln (F("Element %s does not implement handleWebEvent"), id.c_str()); }
    virtual String getData() { iotCore.logWPrintln (F("Element %s does not implement getData"), id.c_str()); return ""; }
    virtual String getError () { return error; }
    virtual void setError (const __FlashStringHelper* _error) {
      if (_error == NULL) {
        error = ""; 
      } else {
        error = String (_error); 
      }
    }

};
#endif 
