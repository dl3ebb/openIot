#include "openIot.h"

extern Preferences prefs;


LedButton::LedButton(String id, int gpio) : 
  Element(id),
  gpioPin(gpio) {
  prefs.end();
  elementManager.registerElement(this);

  prefs.begin ("UiData");
  value = prefs.getBool (id.c_str(),false);
  prefs.end();

  pinMode(gpioPin, OUTPUT);
  setValue(value);
}

/**********************************************************************************
 * MQTT Methods
 **********************************************************************************/
void LedButton::publishMqtt () {
  mqtt.publish (this, "status", value);
}

void LedButton::onMqttConnect () {
  mqtt.subscribe (this, "status");
}

void LedButton::onMqttMessage (const char * topic, const char* mqttValue) {
  if (!strcmp ("status", topic)) {
    if (!strncmp ("true", mqttValue,4)) {
      if (!value) {
        setValue (true);
      }
    } 
    if (!strncmp ("false", mqttValue,5)) {
      if (value) {
        setValue (false);
      }
    } 
  }
}

/**********************************************************************************
 * REST Methods 
 **********************************************************************************/
void LedButton::onRestRequest (const char * payload, AsyncWebServerRequest *request) {
  if (payload != NULL)
  {
    if (!strcasecmp ("on", payload) ||
        !strcasecmp ("true", payload) ||
        !strcasecmp ("1", payload)) {
      setValue(true);
      request->send(OK, "text/plain", "OK");
      return;
    }
    if (!strcasecmp ("off", payload) ||
        !strcasecmp ("false", payload) ||
        !strcasecmp ("0", payload)) {
      setValue(false);
      request->send(OK, "text/plain", "OK");
      return;
    }
  }
  if (value) {
    request->send(OK, "text/plain", "on");
  } else {
    request->send(OK, "text/plain", "off");
  }
}

/**********************************************************************************
 * Web Interface
 **********************************************************************************/
void LedButton::handleWebEvent(int argc, char* argv[]) {
  if (!strcmp (argv[0],"0")) {
    if (value) {
      setValue (false);
      publishMqtt();
    }
  } else {
    if (!value) {
      setValue (true);
      publishMqtt();
    }
  }
}

String LedButton::getData () {
  return (value ? "1":"0");
}

void LedButton::setValue (boolean newValue) {
  value = newValue; 
  //--- Save in NV Ram -------------------------------
  prefs.begin ("UiData");
  prefs.putBool(id.c_str(),value);
  prefs.end();

  //--- Set port -------------------------------------
  if (value) {
    digitalWrite (gpioPin, HIGH);
  } else {
    digitalWrite (gpioPin, LOW);
  }

  ws.textAll (getId() + "|V|" + getData());
}

