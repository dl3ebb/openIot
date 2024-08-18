#include "openIot.h"
#ifndef FORM_TEXT_INPUT_H
#define FORM_TEXT_INPUT_H

class FormTextInput : public Element {
  private:
    String value;
    Form* form;

  public:
    FormTextInput(Form* _form, String _id);

    //--- Web Interface ------------------------------------------------
    bool isWebElement() const override  { return true; }
    void handleWebEvent(int argc, char* argv[]) override ;
    
    String getData() override ;

    void setValue (String _value);
};

#endif 