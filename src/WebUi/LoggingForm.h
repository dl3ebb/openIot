#include "openIot.h"
#ifndef LOGGING_FORM_H
#define LOGGING_FORM_H

class LoggingForm : public Form {

private:
    FormCheckbox* syslogEnable;
    FormTextInput* syslogServer;
    FormSubmitButton* save;

public:
    LoggingForm ();
    bool checkFormData () override;
    void submit () override;
    void begin() override;
};

#endif