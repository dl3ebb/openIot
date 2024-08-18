#include "openIot.h"
#ifndef PROGRESS_DIALOG_H
#define PROGRESS_DIALOG_H

class ProgressDialog : public Element {
  private:
    int32_t low;
    int32_t high;
    int32_t current;

  public:
    ProgressDialog(String _id);
    //--- Web Interface ------------------------------------------------
    bool isWebElement() const override { return true; }
    void handleWebEvent(int argc, char* argv[]) override {}
    String getData() override { return ""; };

    void setLabel (String _label);
    void setLimits (int32_t _low, int32_t _high);
    void updateProgress (int32_t);
    void show();
    void hide();
};

#endif 