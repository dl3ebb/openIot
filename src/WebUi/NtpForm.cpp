#include "openIot.h"

extern IotCore iotCore;
extern ElementManager elementManager;

NtpForm::NtpForm () : 
  Form () {
    ntpEnable = new FormCheckbox (this,"_SYS_NTP_ENABLE");
    ntpServer1 = new FormTextInput (this,"_SYS_NTP_SERVER1");
    ntpServer2 = new FormTextInput (this,"_SYS_NTP_SERVER2");
    ntpOffsetGmt = new FormIntInput (this,"_SYS_NTP_OFFSET_GMT");
    ntpOffsetDst = new FormIntInput (this,"_SYS_NTP_OFFSET_DST");
    save  = new FormSubmitButton (this,"_SYS_NTP_SAVE");

}

bool NtpForm::checkFormData () {
  bool hasError = false;

  if (ntpEnable->getValue()) {
    if (ntpServer1->getData() == "") {
      ntpServer1->setError (F("Server may not be empty"));
      hasError = true;
    } else {
      ntpServer1->setError(NULL);
    }
  } else {
      ntpServer1->setError(NULL);
  }

  if (hasError) {
    save->disable();
    return false;
  } else {
    save->enable();
    return true;
  }
}

void NtpForm::begin() {
  ntpEnable->setValue(iotCore.ntpEnable);
  ntpServer1->setValue(iotCore.ntpServer1);
  ntpServer2->setValue(iotCore.ntpServer2);
  ntpOffsetGmt->setValue(iotCore.ntpOffsetGmt);
  ntpOffsetDst->setValue(iotCore.ntpOffsetDst);
  checkFormData();
}

void NtpForm::submit () {
  if (checkFormData()) {
    iotCore.ntpEnable = ntpEnable->getValue();
    if (iotCore.ntpEnable) {
      iotCore.ntpServer1 = ntpServer1->getData();
      iotCore.ntpServer2 = ntpServer2->getData();
      iotCore.ntpOffsetGmt = ntpOffsetGmt->getValue();
      iotCore.ntpOffsetDst = ntpOffsetDst->getValue();
    } else {
      iotCore.ntpServer1 = "";
      iotCore.ntpServer2 = "";
      iotCore.ntpOffsetGmt = 0;
      iotCore.ntpOffsetDst = 0;
    }
    iotCore.writePreferences (true);
  }
}
