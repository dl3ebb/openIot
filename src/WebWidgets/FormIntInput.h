#include "openIot.h"
#ifndef FORM_INT_INPUT_H
#define FORM_INT_INPUT_H

class FormIntInput : public Element {
  private:
    int value;
    Form* form;

  public:
    FormIntInput(Form* _form, String _id);

    //--- Web interface ------------------------------------------------
    bool isWebElement() const override  { return true; }
    void handleWebEvent(int argc, char* argv[]) override ;
    
    String getData() override ;

    void setValue (int _value);
    int getValue () {return value; }
};

#endif 