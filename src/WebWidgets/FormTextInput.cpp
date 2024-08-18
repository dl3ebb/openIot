#include "openIot.h"

extern Preferences prefs;

FormTextInput::FormTextInput(Form* _form, String _id) : 
    Element(_id),
    form(_form),
    value ("") {
      elementManager.registerElement(this);
      form->registerFormElement (this);      
}

void FormTextInput::handleWebEvent(int _argc, char* _argv[]) {
  if (_argc > 0 && !strcmp("V",_argv[0])) {
    if (_argc == 2) {
      setValue (_argv[1]);
    } else {
      setValue ("");
      this->value = "";
    }
    form->_checkFormData ();
  } 
}

void FormTextInput::setValue (String newValue) {
  if (newValue != this->value) {
    this->value = newValue; 
    ws.textAll (getId() + "|V|" + getData());
  }
}

String FormTextInput::getData () {
  return value;
}
