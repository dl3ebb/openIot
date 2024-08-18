#include "openIot.h"
#include <nvs_flash.h>

extern IotCore iotCore;
extern ElementManager elementManager;

RebootForm::RebootForm () : 
  Form () {
    reboot = new FormButton ("_SYS_REBOOT_REBOOT");
    reset = new FormButton ("_SYS_REBOOT_FACTORY_RESET");
}

void RebootForm::begin() {
  reboot->onClick ([]() {
    iotCore.doReboot();
  });

  reset->onClick ([]() {
    nvs_flash_erase();
    nvs_flash_init();
    iotCore.doReboot();
  });
}
