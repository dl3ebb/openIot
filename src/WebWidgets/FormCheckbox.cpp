#include "openIot.h"

extern Preferences prefs;

FormCheckbox::FormCheckbox(Form* _form, String _id) : 
    Element(_id),
    form(_form),
    value(false) {
      elementManager.registerElement(this);
      form->registerFormElement (this);
}

void FormCheckbox::handleWebEvent(int _argc, char* _argv[]) {
  if (_argc > 1 && !strcmp("V",_argv[0])) {
    setValue (!strcmp("1",_argv[1]));
    form->_checkFormData ();
  } 
}

void FormCheckbox::setValue (bool newValue) {
  value = newValue; 
  ws.textAll (getId() + "|V|" + getData());
}

String FormCheckbox::getData () {
  return (value ? "1":"0");
}


