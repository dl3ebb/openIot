#include "openIot.h"

extern IotCore iotCore;
extern ElementManager elementManager;

LoggingForm::LoggingForm () : 
  Form () {
    syslogEnable = new FormCheckbox (this,"_SYS_SYSLOG_ENABLE");
    syslogServer = new FormTextInput (this,"_SYS_SYSLOG_SERVER"); 
    save = new FormSubmitButton (this,"_SYS_SYSLOG_SAVE");
}

bool LoggingForm::checkFormData () {
  bool hasError = false;

  if (syslogEnable->getValue()) {
    if (syslogServer->getData() == "") {
      syslogServer->setError (F("Servername may not be empty."));
      hasError = true;
    } else {
      syslogServer->setError (NULL);
    }
  } else {
    syslogServer->setError (NULL);
  }

  if (hasError) {
    save->disable();
    return false;
  } else {
    save->enable();
    return true;
  }
}

void LoggingForm::begin() {
  syslogEnable->setValue (iotCore.syslogEnable);
  syslogServer->setValue(iotCore.syslogServer);
  checkFormData();
}

void LoggingForm::submit () {
  if (checkFormData()) {
    iotCore.syslogEnable = syslogEnable->getValue();
    if (iotCore.syslogEnable) {
      iotCore.syslogServer = syslogServer->getData();
    } else {
      iotCore.syslogServer = "";
    }
    iotCore.writePreferences (true);
  }
}
