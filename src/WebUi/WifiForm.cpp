#include "openIot.h"

extern IotCore iotCore;
extern ElementManager elementManager;

WifiForm::WifiForm () : 
  Form () {
    enablePrimary = new FormCheckbox (this, "_SYS_WIFI_ENABLE_PRIMARY");
    ssid1 = new FormSelectbox (this,"_SYS_WIFI_SSID1");
    password1 = new FormTextInput(this,"_SYS_WIFI_PASSWORD1");
    enableSecondary = new FormCheckbox (this, "_SYS_WIFI_ENABLE_SECONDARY");
    ssid2 = new FormSelectbox (this,"_SYS_WIFI_SSID2");
    password2 = new FormTextInput(this,"_SYS_WIFI_PASSWORD2");
    save = new FormSubmitButton(this,"_SYS_WIFI_SUBMIT");
}

bool WifiForm::checkFormData () {
  bool hasError = false;

  if (enablePrimary->getValue()){
    if (ssid1->getData() == "" ) {
      ssid1->setError (F("SSID may not be empty"));
      hasError = true;
    } else {
      ssid1->setError (NULL);
    }
  }

  if (enableSecondary->getValue()) {
    if (ssid2->getData() == "" ) {
      ssid2->setError (F("SSID may not be empty"));
      hasError = true;
    } else {
      ssid2->setError (NULL);
    }
  }

  if (hasError) {
    save->disable();
    return false;
  } else {
    save->enable();
    return true;
  }


}

void WifiForm::begin() {
  // create a options list from the Network scan 
  std::vector<SelectboxOption> wifiNetworks;
  for (const auto& network : iotCore.networkMap) {
    SelectboxOption option;
    option.value = network.second.ssid;
    option.text = network.second.description;
    wifiNetworks.push_back(option);
  }
  enablePrimary->setValue (iotCore.wifi_enable_primary);
  ssid1->setValue (iotCore.wifi_ssid1);
  ssid1->setOptions(wifiNetworks);
  password1->setValue (iotCore.wifi_password1);
  enableSecondary->setValue (iotCore.wifi_enable_secondary);
  ssid2->setValue (iotCore.wifi_ssid2);
  ssid2->setOptions(wifiNetworks);
  password2->setValue (iotCore.wifi_password2);

  checkFormData();
}

void WifiForm::submit () {
  if (checkFormData()) {
    iotCore.wifi_enable_primary = enablePrimary->getValue();
    if (iotCore.wifi_enable_primary) {
      iotCore.wifi_ssid1 = ssid1->getValue();
      iotCore.wifi_password1 = password1->getData();
    } else {
      iotCore.wifi_ssid1 = "";
      iotCore.wifi_password1 = "";
    }

    iotCore.wifi_enable_secondary = enableSecondary->getValue();
    if (iotCore.wifi_enable_secondary) {
      iotCore.wifi_ssid2 = ssid2->getValue();
      iotCore.wifi_password2 = password2->getData();
    } else {
      iotCore.wifi_ssid2 = "";
      iotCore.wifi_password2 = "";
    }
    iotCore.writePreferences (true);
  }
}
