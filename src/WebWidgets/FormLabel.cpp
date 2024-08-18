#include "openIot.h"

extern Preferences prefs;

FormLabel::FormLabel(String _id, std::function<void()> _onClick) : 
    Element(_id),
    onClick(_onClick),
    value("") {
      elementManager.registerElement(this);
}

void FormLabel::handleWebEvent(int _argc, char* _argv[]) {
  if (_argc > 0 && !strcmp("C",_argv[0])) {
    if (onClick) {
      onClick();
    }
  } 
}

void FormLabel::setValue (String newValue) {
  value = newValue; 
  ws.textAll (getId() + "|V|" + getData());
}

String FormLabel::getData () {
  return (value);
}


