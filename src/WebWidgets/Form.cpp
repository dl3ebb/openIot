#include "openIot.h"

void Form::_checkFormData () {
     checkFormData ();

    String data = "";
    boolean first = true;

    for (auto element : formElements) {
        if (element->isWebElement()) {
            if (first) {
                first = false;
            } else {
                data += "@@";
            }
            data += element->getId() + "|E|" + element->getError();
        }
    }
    ws.textAll (data);
}
