#include "openIot.h"

extern Preferences prefs;

FormIntInput::FormIntInput(Form* _form, String _id) : 
    Element(_id),
    form(_form),
    value (0) {
      elementManager.registerElement(this);
      form->registerFormElement (this);
}

void FormIntInput::handleWebEvent(int _argc, char* _argv[]) {
  if (_argc > 0 && !strcmp("V",_argv[0])) {
    if (_argc == 2) {
      setValue (atoi(_argv[1]));
    } else {
      setValue (0);
      this->value = 0;
    }
    form->_checkFormData ();
  } 
}

void FormIntInput::setValue (int newValue) {
  if (newValue != this->value) {
    this->value = newValue; 
    ws.textAll (getId() + "|V|" + getData());
  }
}

String FormIntInput::getData () {
  return String(value);
}

