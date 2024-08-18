#include "openIot.h"
#ifndef LED_BUTTON_H
#define LED_BUTTON_H

class LedButton : public Element  {
  private:
    boolean value;
    int gpioPin;

  public:
    LedButton(String id, int gpio);

    //--- Mqtt Interface -----------------------------------------------
    bool isMqttElement() const override { return true; }
    void publishMqtt () override ;
    void onMqttConnect () override ;
    void onMqttMessage (const char * topic, const char* value) override ;

    //--- Rest interface -----------------------------------------------
    bool isRestElement() const override { return true; }
    void onRestRequest (const char * payload, AsyncWebServerRequest *request) override;

    //--- Web Interface ------------------------------------------------
    bool isWebElement() const override { return true; }
    void handleWebEvent(int argc, char* argv[]) override; 
    String getData() override;

    void setValue (boolean value);


};

#endif 