#include "openIot.h"
#ifndef MQTT_FORM_H
#define MQTT_FORM_H

class MqttForm : public Form {

private:
    FormCheckbox* enableMqtt;
    FormTextInput* mqttServer;
    FormIntInput* mqttPort;
    FormTextInput* mqttClientId;
    FormTextInput* mqttUserName;
    FormTextInput* mqttPassword;
    FormTextInput* mqttPrefix;
    FormSubmitButton* save;

public:
    MqttForm ();
    bool checkFormData () override;
    void submit () override;
    void begin() override;
};

#endif // ELEMENTMANAGER_H
