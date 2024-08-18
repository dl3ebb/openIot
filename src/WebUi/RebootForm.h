#include "openIot.h"
#ifndef REBOOT_FORM_H
#define REBOOT_FORM_H

class RebootForm : public Form {

private:
    FormButton* reboot;
    FormButton* reset;

public:
    ProgressDialog* progressDialog;
    RebootForm ();
    bool checkFormData () override { return true; };
    void submit () override {};
    void begin() override;

};

#endif