#include "openIot.h"
#ifndef FORM_SUBMITBUTTON_H
#define FORM_SUBMITBUTTON_H

class FormSubmitButton : public Element {
  private:
    Form *form;

  public:
    FormSubmitButton (Form* _form, String _id);

    //--- Web interface ------------------------------------------------
    bool isWebElement() const override  { return true; }
    void handleWebEvent(int argc, char* argv[]) override ;
    String getData() override  {return ""; }
    String getError() override  {return ""; }


    void enable ();
    void disable ();
};

#endif 