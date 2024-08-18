#include "openIot.h"

extern Preferences prefs;

ProgressDialog::ProgressDialog(String _id) : 
    Element(_id) {
      elementManager.registerElement(this);
}

void ProgressDialog::setLabel (String _label) {
  ws.textAll (getId() + "|LAB|" + _label);
};

void ProgressDialog::setLimits (int32_t _low, int32_t _high) {
  low = _low;
  high = _high;
};

void ProgressDialog::updateProgress (int32_t progress) {
  int32_t percentage = ((progress - low) * 100) / (high - low);
  if (percentage != current) {
    current = percentage;
    ws.textAll (getId() + "|VAL|" + current);
  }
};

void ProgressDialog::show() {
  ws.textAll (getId() + "|SHOW");
}

void ProgressDialog::hide() {
  ws.textAll (getId() + "|HIDE");
}
