#include "openIot.h"
#include <HTTPClient.h>
#include <Update.h>
#include <AsyncDelay.h>
#include <esp_task_wdt.h>

extern UpdateServerForm updateServerForm;
Ticker webUpdateTicker;

void webUpdateCheck () {
  webUpdate.updateCheck();
}


void WebUpdate::updateCheck () {
  if (checkForUpdate() && iotCore.autoUpdate && iotCore.netStatus == NET_STA) {
      doUpdate(nullptr);
    }
}

void WebUpdate::begin () {
    updateCheck();
    webUpdateTicker.attach(3600, ::webUpdateCheck);
}

/**
 * Check if there is a newer version on the server 
 */
bool WebUpdate::checkForUpdate () {
  if (iotCore.updateServerEnable && iotCore.updateServer != "" && iotCore.netStatus == NET_STA) {
    HTTPClient http;
    AsyncDelay updateAsyncDelay;

    //--- Build the URL for the update script --------------------------------------
    String url = "http://" + iotCore.updateServer + "/uq.php?app=" + iotCore.applicationName;
    http.begin(url);
    int httpCode = http.GET();

    //--- Web call failed. Log the reason and exit ---------------------------------
    if (httpCode != HTTP_CODE_OK) {
      iotCore.logWPrintln(F("Check for Update. failed for %s. Code : %d"), url.c_str(), httpCode );
      http.end ();
      return false; 
    }

    //--- We got a response from the server -----------------------------------------
    String payload = http.getString();
    updateVersion = payload.toInt();
    http.end();

    if (updateVersion <= iotCore.applicationBuild) {
      iotCore.logIPrintln(F("No update. Installed Version : %d Version on Update Server : %d"), iotCore.applicationBuild, updateVersion );
      return false;
    }
    iotCore.logIPrintln(F("Update available. Installed Version : %d Version on Update Server : %d"), iotCore.applicationBuild, updateVersion );
    return true;
  }
  return false;
}

/**
 * This is the update Task. 
 */
void doUpdate (void *parameter) {
  updateServerForm.progressDialog->show();
  if (webUpdate.performUpdate(false) && webUpdate.performUpdate(true)) {
    updateServerForm.progressDialog->hide();
    iotCore.logIPrintln(F("Update succeeded. Restart device."));
    delay(1000); // wait for the message to be delivered to the server
    iotCore.doReboot();
  }
}

/**
 * This is the actual update. For a complete update the method is called 
 * twice. Once for the Binary and once for the SPIFFS image.
 */
bool WebUpdate::performUpdate (bool updateFlash) {

  if (updateFlash) {
    updateServerForm.progressDialog->setLabel(F("Downloading Binary."));
  } else {
    updateServerForm.progressDialog->setLabel(F("Downloading SPIFFS Image"));
  }
  iotCore.logWPrintln(F("Heap : %ld"), esp_get_free_heap_size());
  //--- Timeout for the download of one packet. --------------------------
  esp_task_wdt_init(10, true);

  int buffer = 32767;
  if (buffer > esp_get_free_heap_size()) {
    iotCore.logWPrintln(F("Not enough heap size. Update aborted"));
    return false;
  }

  //--- Build the URL --------------------------------
  String firmwareUrl = "http://" + iotCore.updateServer + "/update/img/" + iotCore.applicationName + "_" + updateVersion + (updateFlash ? ".bin" : ".img");

  HTTPClient http;
  http.begin(firmwareUrl);
  int httpCode = http.GET();
  if (httpCode != 200) {
    iotCore.logWPrintln(F("Download of new firmware %s failed (Code %d)!"), firmwareUrl.c_str(), httpCode);
    http.end();
    return false;
   } 
    
  int totalLength = http.getSize();
  iotCore.logIPrintln(F("Updating from %s (Length = %ld bytes)"), firmwareUrl.c_str(), totalLength);
  updateServerForm.progressDialog->setLimits(0,totalLength);

  Update.begin(UPDATE_SIZE_UNKNOWN, updateFlash ? U_FLASH : U_SPIFFS);
  int len = totalLength;
  int  currentLength = 0;
  uint8_t *buff = new uint8_t[buffer];
  
  WiFiClient *stream = http.getStreamPtr();

  //--- The absolute timeout for the update process (2 Minutes)---- 
  AsyncDelay updateAsyncDelay;
  updateAsyncDelay.start(120000, AsyncDelay::MILLIS);
  while (http.connected() && (len > 0 || len == -1)) {
    if (updateAsyncDelay.isExpired()) {
      http.end();
      iotCore.logWPrintln (F("Update failed (Timeout)!"));
      updateServerForm.progressDialog->hide();
      return false;
    }

    updateServerForm.progressDialog->updateProgress (currentLength);
    
    esp_task_wdt_reset();
    vTaskDelay(pdMS_TO_TICKS(100)); 

    size_t size = stream->available();
    if (size) {
      uint16_t c = stream->readBytes(buff, ((size > buffer) ? buffer : size));
      Update.write(buff, c);
      currentLength += c;

      if (len > 0)
        len -= c;

      if (currentLength >= totalLength) {
        http.end();

        updateServerForm.progressDialog->setLabel(F("Writing flash !"));
        bool ok = Update.end(true);
        if (!ok) {
          iotCore.logWPrintln(F("Update failed !"));
          return false;
        }
        iotCore.logWPrintln(F("Update ok !"));
        return true;
       }
    }
  }

  /**
   * Reset Task Timer to 1 Second
   */
  esp_task_wdt_init(1, true);

}
