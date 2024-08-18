#include "openIot.h"

extern Preferences prefs;

FormSubmitButton::FormSubmitButton(Form* _form, String _id) : 
    Element(_id),
    form (_form) {
      elementManager.registerElement(this);
      form->registerFormElement (this);
}

void FormSubmitButton::handleWebEvent(int _argc, char* _argv[]) {
  if (_argc = 1 && !strcmp("C",_argv[0])) {
    form->submit ();
  } 
}

void FormSubmitButton::enable () {
  ws.textAll (getId() + "|EN");
}

void FormSubmitButton::disable () {
  ws.textAll (getId() + "|DI");
}