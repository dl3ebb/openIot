#include "openIot.h"
#ifndef FORM_CHECKBOX_H
#define FORM_CHECKBOX_H

class FormCheckbox : public Element {
  private:
    bool value;
    Form *form;

  public:
    FormCheckbox(Form* _form, String _id);

    bool isWebElement() const override { return true; }
    void handleWebEvent(int argc, char* argv[]) override;
    String getData() override;

    bool getValue () { return value; };
    void setValue (bool _value);
};

#endif 