#include "openIot.h"
#ifndef DEVICE_FORM_H
#define DEVICE_FORM_H

class DeviceForm : public Form {

private:
    FormTextInput* deviceName;
    FormSubmitButton* save;

public:
    DeviceForm ();
    bool checkFormData () override;
    void submit () override;
    void begin() override;
};

#endif