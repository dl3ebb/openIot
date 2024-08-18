#include "openIot.h"
#ifndef COUNTDOWN_DIALOG_H
#define COUNTDOWN_DIALOG_H

class CountdownDialog : public Element {
  public:
    CountdownDialog(String _id);
    //--- Web interface ------------------------------------------------
    bool isWebElement() const override { return true; }
    void handleWebEvent(int argc, char* argv[]) override {}
    String getData() override { return ""; };

    void start ();
};

#endif 