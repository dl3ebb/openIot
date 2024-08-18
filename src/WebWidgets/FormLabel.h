#include "openIot.h"
#include <functional>
#ifndef FORM_LABEL_H
#define FORM_LABEL_H

class FormLabel : public Element {

  private:
    String value;
    std::function<void()> onClick;
    
  public:
    FormLabel(String _id, std::function<void()> _onClick=nullptr);

    //--- Web Interface ------------------------------------------------
    bool isWebElement() const override { return true; }
    void handleWebEvent(int argc, char* argv[]) override;
    String getData() override;

    String getValue () { return value; };
    void setValue (String _value);
};

#endif 