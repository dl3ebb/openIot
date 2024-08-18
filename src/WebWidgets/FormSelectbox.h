#include "openIot.h"
#include <vector>
#ifndef FORM_SELECTBOX_H
#define FORM_SELECTBOX_H

struct SelectboxOption {
  String value;
  String text;
};

class FormSelectbox : public Element {
  private:
    String value;
    std::vector<SelectboxOption> options;

    Form* form;

  public:
    FormSelectbox(Form* _form, String _id);

    //--- Web interface ------------------------------------------------
    bool isWebElement() const override  { return true; }
    void handleWebEvent(int argc, char* argv[]) override ;
    String getData() override ;

    void setValue (String _value);
    String getValue () {return value; }
    void setOptions (std::vector<SelectboxOption> options);
};

#endif 