#include "openIot.h"
#ifndef MQTT_H
#define MQTT_H

class Element;

class Mqtt {
	
	public:
		void begin (const char * mqttServer, int mqttPort, const char *  _mqttClientId, const char *  _mqttUserName, const char *  _mqttPassword, const char *  _mqttPrefix);
		void publish (Element *element, const char * subtopic, boolean value);
		void publish (const char * topic, const char * subtopic, const char * value);
		void publish (const char * topic, const char * subtopic, int value);

		void subscribe (Element *element, const char * subtopic);
		void reconnect();
		void onMqttMessage (char *topic, byte *payload, unsigned int length);
		void loop();

	private:
		const char * mqttPrefix;
		const char * mqttClientId;
		const char * mqttUserName;
		const char * mqttPassword;

};
#endif