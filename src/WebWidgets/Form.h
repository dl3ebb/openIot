#include "openIot.h"
#ifndef FORM_H
#define FORM_H
#include <vector>

class Form {
    private:
        std::vector<Element*> formElements;

public:
    void _checkFormData ();
    void registerFormElement (Element* _element) {
        formElements.push_back(_element);
    }

    virtual bool checkFormData () = 0;
    virtual void submit () = 0;
    virtual void begin () = 0;
};

#endif 
