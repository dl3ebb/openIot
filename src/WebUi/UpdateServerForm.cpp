#include "openIot.h"

extern IotCore iotCore;
extern ElementManager elementManager;

UpdateServerForm::UpdateServerForm () : 
  Form () {
    updateServerEnable = new FormCheckbox (this,"_SYS_UPDATE_SERVER_ENABLE");
    updateServer = new FormTextInput (this,"_SYS_UPDATE_SERVER" ); 
    autoUpdate = new FormCheckbox (this,"_SYS_UPDATE_SERVER_AUTOUPDATE");
    save = new FormSubmitButton (this,"_SYS_UPDATE_SERVER_SAVE");
    software = new FormLabel ("_SYS_UPDATE_SERVER_SOFTWARE");
    installedVersion = new FormLabel ("_SYS_UPDATE_SERVER_VERSION_INSTALLED");
    availableVersion = new FormLabel ("_SYS_UPDATE_SERVER_VERSION_AVAILABLE");
	  doUpdate = new FormButton ("_SYS_UPDATE_SERVER_UPDATE");
    progressDialog = new ProgressDialog ("_SYS_UPDATE_PROGRESS");
}

bool UpdateServerForm::checkFormData () {
  bool hasError = false;

  if (updateServerEnable->getValue()) {
    if (webUpdate.updateVersion > iotCore.applicationBuild) {
      doUpdate->enable();
    } else {
      doUpdate->disable();
    }
    if (updateServer->getData() == "") {
      updateServer->setError (F("Servername may not be empty."));
      hasError = true;
    } else {
      updateServer->setError (NULL);
    }
  } else {
    doUpdate->disable();
    updateServer->setError (NULL);
  }

  if (hasError) {
    save->disable();
    return false;
  } else {
    save->enable();
    return true;
  }
}

void UpdateServerForm::rescanServer () {
  webUpdate.checkForUpdate();
  availableVersion->setValue (String (webUpdate.updateVersion));
}

void UpdateServerForm::begin() {
  updateServerEnable->setValue (iotCore.updateServerEnable);
  updateServer->setValue(iotCore.updateServer);
  autoUpdate->setValue (iotCore.autoUpdate);
  software->setValue(iotCore.applicationName);
  installedVersion->setValue (String (iotCore.applicationBuild));
  rescanServer();
  checkFormData();

  //--- The actual update is done by the main loop to avoid wdt timeouts ----
  doUpdate->onClick ([]() {
    iotCore.logIPrintln (F("Processing Update."));
    Task newTask = {::doUpdate, (void *)""};   
    xQueueSend(taskQueue, &newTask, portMAX_DELAY);
  });
}

void UpdateServerForm::submit () {
  if (checkFormData()) {
    iotCore.updateServerEnable = updateServerEnable->getValue();
    if (iotCore.updateServerEnable) {
      iotCore.updateServer = updateServer->getData();
      iotCore.autoUpdate = autoUpdate->getValue();
    } else {
      iotCore.updateServer = "";
      iotCore.autoUpdate = false;
    }
    iotCore.writePreferences (true);
  }
}
