#include "openIot.h"
#include <functional>
#ifndef FORM_BUTTON_H
#define FORM_BUTTON_H

class FormButton : public Element {
  private:
    Form *form;
    std::function<void()> _onClick;
    bool enabled;

  public:
    FormButton (String _id);

    //--- Web interface ------------------------------------------------
    void onClick (std::function<void()> _onClick);
    bool isWebElement() const override  { return true; }
    void handleWebEvent(int argc, char* argv[]) override ;
    String getData() override;
    String getError() override  {return ""; }

    void enable ();
    void disable ();
};

#endif 