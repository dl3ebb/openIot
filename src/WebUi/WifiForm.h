#include "openIot.h"
#ifndef WIFI_FORM_H
#define WIFI_FORM_H

class WifiForm : public Form {

private:
    FormCheckbox* enablePrimary;
    FormSelectbox* ssid1;
    FormTextInput* password1;
    FormCheckbox* enableSecondary;
    FormSelectbox* ssid2;
    FormTextInput* password2;
    FormSubmitButton* save;

public:
    WifiForm ();
    bool checkFormData () override;
    void submit () override;
    void begin() override;
};

#endif // ELEMENTMANAGER_H
