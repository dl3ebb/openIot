#include "openIot.h"

WiFiClient espClient;
PubSubClient client(espClient);

/**
 * Callback for incoming MQTT messages
 */
void callback(char *topic, byte *payload, unsigned int length) {
    mqtt.onMqttMessage (topic, payload, length);
}

void Mqtt::onMqttMessage (char *topic, byte *payload, unsigned int length) {
    char cPayLoad[length+1];
    for (int i = 0; i < length; i++) {
        cPayLoad[i] = (char) payload[i];
    }
    cPayLoad[length] = 0;
    
    if (strncmp(topic, mqttPrefix, strlen(mqttPrefix)) == 0) {
        const char* elemTopic = topic + strlen(mqttPrefix) + 5;
        elementManager.onMqttMessage (elemTopic, cPayLoad);
    }
}

void Mqtt::begin (const char * mqttServer, int mqttPort, const char *  _mqttClientId, const char *  _mqttUserName, const char *  _mqttPassword, const char *  _mqttPrefix) {
    mqttPrefix = _mqttPrefix;
    mqttClientId = _mqttClientId;
    mqttUserName = _mqttUserName;
    mqttPassword = _mqttPassword;

    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

/**
 * this method publishes element data to the MQTT server
 * Zhe Topic is built from <mqttPrefix>/<elementId>/<subtopic>.
 */ 
void Mqtt::publish (Element *_element, const char * _subtopic, boolean _value) {
    char topic[200];
    sprintf (topic,"%s/app/%s/%s", mqttPrefix, _element->getId(),_subtopic);
    client.publish(topic, _value ? "true" : "false",true);
}

/**
 * These methods are used to publish core datat to MQTT
 */
void Mqtt::publish (const char * _topic, const char * _subtopic, const char * _value) {
    char topic[200];
    sprintf (topic,"%s/%s", _topic,_subtopic);
    client.publish(topic, _value,true);
}

void Mqtt::publish (const char * _topic, const char * _subtopic, int _value) {
    char topic[200];
    char value[10];
    sprintf (topic,"%s/%s", _topic,_subtopic);
    sprintf(value,"%d", _value);
    client.publish(topic, value);
}

/**
 * This methods is used to subscribe to a topic
 */
void Mqtt::subscribe (Element *_element, const char * _subtopic) {
    char topic[200];
    sprintf (topic,"%s/app/%s/%s", mqttPrefix, _element->getId(),_subtopic);
    client.subscribe(topic);
}

/**
 * This method is called to (re)connect to a MQTT Server. If a new connection is 
 * established, the elementmanager is noticed so he can publish / subscribe the element data
 */
void Mqtt::reconnect() {
    if (iotCore.getNetStatus() == NET_STA && iotCore.mqttEnable) {
        while (!client.connected()) {
            if (client.connect(mqttClientId, mqttUserName, mqttPassword)) {
                iotCore.logWPrintln (F("Mqtt Connection established."));
                elementManager.onStartMqtt();
                return;
            } else {
                iotCore.logWPrintln (F("Mqtt Connection failed with state %d"), client.state());
                delay(1000);
            }
        }
    }
}

void Mqtt::loop() {
    if (!client.connected()) {
        reconnect();
    }    
    client.loop();
}
