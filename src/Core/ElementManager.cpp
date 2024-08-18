#include "openIot.h"
/**
 * The Element Manager holds all Elements in an application and dispatches the different events to them. 
 */

extern IotCore iotCore;

void ElementManager::registerElement(Element* _element) {
    elements.push_back(_element);
}


/*****************************************************************************
 * Rest Interface 
 *****************************************************************************/
void ElementManager::initRest () {
  webServer.on("/api", HTTP_GET, [](AsyncWebServerRequest *request) {
    elementManager.handleRestRequest (request);
  });
}

/**
 * Rest request are always in the format /api/<id>/<payload>
 * This method gets the request, solit it up and sends it to the element fpr processing
 */
void ElementManager::handleRestRequest (AsyncWebServerRequest* _request) {

    char * payload = NULL;

    if (_request->url().length() > 500) {
      _request->send(401, "text/html", "Request too long !");
      return;
    }

    char urlBuffer[500+1];
    strcpy (urlBuffer, _request->url().c_str());
    char *elementId = urlBuffer + 5;

    char* endId = strchr (elementId,'/');
    if (endId != NULL) { 
        *endId = 0;
        payload = endId +1;
    }

    for (auto element : elements) {
      if (element->getId() == elementId) {
        if (element->isRestElement()) {
          element->onRestRequest(payload, _request);
        } else {
          iotCore.logWPrintln (F("HandleRestRequest : Element with id %s does not implement RestElement Interface!"), element->getId());
        }
        return;
      }
    }

    _request->send(404, "text/html", "Not found");
}

/***************************************************************************************
 * Websocket Interface 
 ***************************************************************************************/
/**
 * Websockets are similar to the Rest Request. The format is <id>|<payload>
 * Payload may consist of multiple argumens seperated by "|". The method splits these up and 
 * sends it to the element as argc / argv[].
 */
void  ElementManager::handleWebEvent(const String& _event) {
  // String split into id and arguments
  int idEndIndex = _event.indexOf('|');
  if (idEndIndex == -1) {
    iotCore.logEPrintln (F("Invalid event format %s"), _event.c_str());
    return;
  }

  String id = _event.substring(0, idEndIndex);
  String args = _event.substring(idEndIndex + 1);

  // Split args into argv array
  std::vector<char*> argv;
  int argc = 0;
  char* token = strtok(const_cast<char*>(args.c_str()), "|");
  while (token != nullptr) {
    argv.push_back(token);
    token = strtok(nullptr, "|");
    argc++;
  }

  // Find element by id and handle event
  for (auto element : elements) {
    if (element->getId() == id) {
      if (element->isWebElement()) {
        element->handleWebEvent(argc, argv.data());
        return;
      } else {
          iotCore.logWPrintln (F("HandleWebRequest : Element with id %s does not implement WebElement Interface!"), element->getId());
      }
    }
  }
  iotCore.logEPrintln (F("No element found with ID: %s"),id.c_str());
}

/**
 * If a new Websocket client registers to the backend, it needs the data for the page. 
 * This method collects all data from all elements and sends it to the frontend. 
 * The Format for each element is <id>|data. The element data is seperated by "@@". 
 */
void  ElementManager::sendValues (unsigned int _clientId) {
  String data = "";
  boolean first = true;

  for (auto element : elements) {
    if (element->isWebElement()) {
      if (first) {
          first = false;
        } else {
          data += "@@";
        }
        data += element->getId() + "|V|" + element->getData();
    }
  }
  Serial.println(data);
  ws.text (_clientId, data);
}


/***************************************************************************
 * MQTT Interface 
 ***************************************************************************/
/**
 * If a MQTT method is (re)established this method will be called. 
 * First for all elements the method onMqttConnect() will be called. This 
 * should subscribe to the MQTT topics and receive the last data stored there
 * 
 * After this the publish Method is called to make sure, that any local data 
 * is published to the MQTT server. 
 */
void ElementManager::onStartMqtt() {

  for (auto element : elements) {
    if (element->isMqttElement()) {
      element->onMqttConnect();
    }
  }

  for (auto element : elements) {
    if (element->isMqttElement()) {
      element->publishMqtt ();
    }
  }
}

/**
 * This is mehod is called by incoming MQTT messages. 
 * 
 * The payload is in the format of <id>/<elementTopic
 * 
 * The method will split the id/payload, find the element and dispatch the payload to it. 
 */
void ElementManager::onMqttMessage (const char* _topic, const char* _payload) {
  char* endId = strchr (_topic,'/');
  if (endId == NULL) return;
  *endId = 0;

  String elementId = String (_topic);
  char * elemTopic = endId +1;

  for (auto element : elements) {
    if (element->getId() == elementId) {
      if (element->isMqttElement()) {
        element->onMqttMessage(elemTopic, _payload);
        return;
      }
    }
  }
}

