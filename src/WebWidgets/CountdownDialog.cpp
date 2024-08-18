#include "openIot.h"

extern Preferences prefs;

CountdownDialog::CountdownDialog(String _id) : 
    Element(_id) {
    elementManager.registerElement(this);
}

void CountdownDialog::start () {
  ws.textAll (getId() + "|S");
}
