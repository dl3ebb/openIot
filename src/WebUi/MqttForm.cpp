#include "openIot.h"

extern IotCore iotCore;
extern ElementManager elementManager;

MqttForm::MqttForm () : 
  Form () {
    enableMqtt = new FormCheckbox (this,"_SYS_MQTT_ENABLE");
    mqttServer = new FormTextInput (this,"_SYS_MQTT_SERVER");
    mqttPort = new FormIntInput (this,"_SYS_MQTT_SERVER_PORT"); 
    mqttClientId = new FormTextInput (this,"_SYS_MQTT_CLIENT_ID");
    mqttUserName = new FormTextInput (this,"_SYS_MQTT_USER_NAME");
    mqttPassword = new FormTextInput (this,"_SYS_MQTT_PASSWORD");
    mqttPrefix = new FormTextInput (this,"_SYS_MQTT_PREFIX");
    save  = new FormSubmitButton (this,"_SYS_MQTT_SAVE");
}

bool MqttForm::checkFormData () {
  bool hasError = false;

  if (enableMqtt->getValue()) {
    if (mqttServer->getData() == "" ) {
      mqttServer->setError (F("Server may not be empty"));
      hasError = true;
    } else {
      mqttServer->setError (NULL);
    }

    if (mqttPort->getValue() <= 0) {
      mqttPort->setError (F("Invalid port number"));
      hasError = true;
    } else {
      mqttPort->setError (NULL);
    }

    if (mqttClientId->getData() == "" ) {
      mqttClientId->setError (F("Client-Id may not be empty"));
      hasError = true;
    } else {
      mqttClientId->setError (NULL);
    }
  } else {
      mqttServer->setError (NULL);
      mqttPort->setError (NULL);
      mqttClientId->setError (NULL);
  }

  if (hasError) {
    save->disable();
    return false;
  } else {
    save->enable();
    return true;
  }
}

void MqttForm::begin() {
  enableMqtt->setValue (iotCore.mqttEnable);
  mqttServer->setValue (iotCore.mqttServer);
  mqttPort->setValue (iotCore.mqttPort);
  mqttClientId->setValue (iotCore.mqttClientId);
  mqttUserName->setValue (iotCore.mqttUserName);
  mqttPassword->setValue (iotCore.mqttPassword);
  mqttPrefix->setValue (iotCore.mqttPrefix);
  checkFormData();
}

void MqttForm::submit () {
  if (checkFormData()) {
    iotCore.mqttEnable = enableMqtt->getValue();
    if (iotCore.mqttEnable) {
      iotCore.mqttServer = mqttServer->getData();
      iotCore.mqttPort = mqttPort->getValue();
      iotCore.mqttClientId = mqttClientId->getData();
      iotCore.mqttUserName = mqttUserName->getData();
      iotCore.mqttPassword = mqttPassword->getData();
      iotCore.mqttPrefix = mqttPrefix->getData();
    } else {
      iotCore.mqttServer = "";
      iotCore.mqttPort = 1883;
      iotCore.mqttClientId = "";
      iotCore.mqttUserName = "";
      iotCore.mqttPassword = "";
      iotCore.mqttPrefix = "";
    }

    iotCore.writePreferences (true);
  }
}
