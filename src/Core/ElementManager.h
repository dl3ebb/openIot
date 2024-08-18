#include "openIot.h"
#ifndef ELEMENTMANAGER_H
#define ELEMENTMANAGER_H
#include <vector>

class ElementManager {
private:
    std::vector<Element*> elements;

public:
  void registerElement(Element* _element);

  /**
   * REST Interface 
   */
  void initRest();
  void handleRestRequest (AsyncWebServerRequest *_request);

  /**
   * WebSocket Interface 
   */
  void handleWebEvent(const String& _event);
  void sendValues (unsigned int _clientId);

  /**
   * Mqtt Interface 
   */
  void onStartMqtt();
  void onMqttMessage (const char * _topic, const char* _value);
};

#endif // ELEMENTMANAGER_H
