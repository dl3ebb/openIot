#include "openIot.h"
#ifndef UPDATE_SERVER_FORM_H
#define UPDATE_SERVER_FORM_H

class UpdateServerForm : public Form {

private:
    FormCheckbox* updateServerEnable;
    FormTextInput* updateServer;
    FormCheckbox* autoUpdate;
    FormSubmitButton* save;
    FormLabel *software;
    FormLabel *installedVersion;
    FormLabel *availableVersion; 
    FormButton* doUpdate;

public:
    ProgressDialog* progressDialog;
    UpdateServerForm ();

    bool checkFormData () override;
    void submit () override;
    void begin() override;

    void rescanServer ();

};

#endif