#include "openIot.h"

extern Preferences prefs;



FormSelectbox::FormSelectbox(Form* _form, String _id) : 
    Element(_id),
    form(_form),
    value ("") {
      elementManager.registerElement(this);
      form->registerFormElement (this);
}

void FormSelectbox::handleWebEvent(int _argc, char* _argv[]) {
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

void FormSelectbox::setValue (String newValue) {
  this->value = newValue; 
  ws.textAll (getId() + "|V|" + getData());
}

void FormSelectbox::setOptions (std::vector<SelectboxOption> _options) {
  this->options = _options;
  ws.textAll (getId() + "|V|" + getData());
}

String FormSelectbox::getData () {
  String jsonOptions = "[";
  for (const auto& option : options) {
    if (jsonOptions.length() > 1) jsonOptions += ",";
    jsonOptions += "{\"value\":\"" + option.value + "\",\"text\":\"" + option.text + "\"}";
  }
  jsonOptions += "]";

  return value + "|" + jsonOptions;
}

