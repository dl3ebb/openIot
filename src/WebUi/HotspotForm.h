#include "openIot.h"
#ifndef HOTSPOT_FORM_H
#define HOTSPOT_FORM_H

class HotspotForm : public Form {

private:
    FormTextInput* ssid;
    FormTextInput* password;
    FormCheckbox* openAp;
    FormSubmitButton* save;

public:
    HotspotForm ();
    bool checkFormData () override;
    void submit () override;
    void begin() override;
};

#endif // ELEMENTMANAGER_H
