#include "openIot.h"

extern Preferences prefs;

FormButton::FormButton(String _id) : 
    Element(_id), 
    enabled(true) {
      elementManager.registerElement(this);
      _onClick = nullptr;
}

void FormButton::onClick (std::function<void()> __onClick) {
  _onClick = __onClick;
}

void FormButton::handleWebEvent(int _argc, char* _argv[]) {
  if (_argc = 1 && !strcmp("C",_argv[0])) {
    if (_onClick != nullptr)  {
      _onClick ();
    }
  } 
}

String FormButton::getData() {
   if (enabled) {
    return "|EN";
   } else {
    return "|DI";
   }
}

void FormButton::enable () {
  enabled = true;
  ws.textAll (getId() + "|EN");
}

void FormButton::disable () {
  enabled = false;
  ws.textAll (getId() + "|DI");
}