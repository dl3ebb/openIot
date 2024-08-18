#include "openIot.h"
#ifndef NTP_FORM_H
#define NTP_FORM_H

class NtpForm : public Form {

private:
    FormCheckbox* ntpEnable;
    FormTextInput* ntpServer1;
    FormTextInput* ntpServer2;
    FormIntInput* ntpOffsetGmt;
    FormIntInput* ntpOffsetDst;
    FormSubmitButton* save;

public:
    NtpForm ();
    bool checkFormData () override;
    void submit () override;
    void begin() override;
};

#endif 
