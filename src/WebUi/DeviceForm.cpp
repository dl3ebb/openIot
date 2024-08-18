#include "openIot.h"

extern IotCore iotCore;
extern ElementManager elementManager;

DeviceForm::DeviceForm () : 
  Form () {
    deviceName = new FormTextInput (this,"_SYS_DEVICE_NAME");
    save = new FormSubmitButton (this,"_SYS_DEVICE_SAVE");
}

bool DeviceForm::checkFormData () {
  bool hasError = false;

  if (deviceName->getData() == "") {
    deviceName->setError (F("DeviceName may not be empty."));
    hasError = true;
  }

  if (hasError) {
    save->disable();
    return false;
  } else {
    save->enable();
    return true;
  }
}

void DeviceForm::begin() {
  deviceName->setValue(iotCore.hostname);
  checkFormData();
}

void DeviceForm::submit () {
  if (checkFormData()) {
    iotCore.hostname = deviceName->getData();
    iotCore.writePreferences (true);
  }
}
