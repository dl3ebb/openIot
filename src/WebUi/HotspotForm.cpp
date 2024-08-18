#include "openIot.h"

extern IotCore iotCore;
extern ElementManager elementManager;

HotspotForm::HotspotForm () 
: Form () {
  ssid = new FormTextInput (this, "_SYS_HOTSPOT_SSID");
  password = new FormTextInput (this, "_SYS_HOTSPOT_PASSWORD");
  openAp = new FormCheckbox (this, "_SYS_HOTSPOT_OPEN");
  save = new FormSubmitButton (this, "_SYS_HOTSPOT_SUBMIT");
}

bool HotspotForm::checkFormData () {
  bool hasError = false;
  if (ssid->getData() == "" ) {
    ssid->setError (F("SSID may not be empty"));
    hasError = true;
  } else {
    ssid->setError (NULL);
  }

  if (!openAp->getValue()) {
    
    if (password->getData().length() < 8 || password->getData().length() > 63) {
      password->setError (F("Password needs to be between 8 and 63 characters"));
      hasError = true;
    } else {
      password->setError (NULL);
    }
  } else {
    password->setError (NULL);
  }
  
  if (hasError) {
    save->disable();
    return false;
  } else {
    save->enable();
    return true;
  }
}

void HotspotForm::begin() {
  ssid->setValue (iotCore.ap_ssid);
  password->setValue (iotCore.ap_password);
  openAp->setValue (iotCore.ap_open);
  checkFormData();
}

void HotspotForm::submit () {
  if (checkFormData()) {
    iotCore.ap_ssid = ssid->getData();
    iotCore.ap_password = password->getData();
    iotCore.ap_open = openAp->getValue();
    iotCore.writePreferences (true);
  }
}
