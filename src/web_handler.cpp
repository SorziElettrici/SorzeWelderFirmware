#include <web_handler.h>


//
// Webpages functions
//
void serveHomePage(AsyncWebServerRequest *request, bool pumpState, int pumpSpeed, int pressureSensorValue) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m", HTML_MENU);
  html.replace("%b", HTML_HOMEPAGE);
  html.replace("%t", "Control panel");

  html.replace("%s", String(pumpState ? "ON" : "OFF"));
  html.replace("%v", String(pressureSensorValue));
  html.replace("%d", String(pumpSpeed));
  request->send(200, "text/html", html);
}

void serveUtilsPage(AsyncWebServerRequest *request) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m", HTML_MENU);
  html.replace("%b", HTML_UTILSPAGE);
  html.replace("%t", "Utilities");
  request->send(200, "text/html", html);
}

void serveWifiPage(AsyncWebServerRequest *request) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m", HTML_MENU);
  html.replace("%b", HTML_WIFIPAGE);
  html.replace("%t", "Configure WiFi");
  request->send(200, "text/html", html);
}

void serveFirmwareUpdatePage(AsyncWebServerRequest *request) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m", HTML_MENU);
  html.replace("%b", HTML_OTAPAGE);
  html.replace("%t", "Firmware Update");
  request->send(200, "text/html", html);
}


//
// PUMP Web functions
//
void handlePostPumpSpeed(AsyncWebServerRequest *request, int &pumpSpeed, bool &pumpState, Preferences &preferences) {
  int newPumpSpeed = request->arg("pumpspeed").toInt();

  if (newPumpSpeed < 0) {
    newPumpSpeed = 0;
  } else if (newPumpSpeed > 255) {
    newPumpSpeed = 255;
  }

  setPumpSpeed(pumpSpeed, newPumpSpeed, pumpState, preferences);
  request->send(200, "text/plain", "Pump speed saved.");
}

void handlePostTogglePump(AsyncWebServerRequest *request, int &pumpSpeed, bool &pumpState, Preferences &preferences) {
  togglePumpState(pumpSpeed, pumpState);
  request->send(200, "text/plain", "Pump toggled.");
}


//
// Command handling functions
//
void handleWifiCredentialsSave(AsyncWebServerRequest *request, Preferences &preferences) {
  String ssid = request->arg("ssid");
  String password = request->arg("password");
  if (ssid.length() > 0 && password.length() > 0) {
    Serial.println("Saving new WiFi credentials...");
    preferences.begin("wifi_cred", false);  // Start using the Preferences
    preferences.putString("WF_SSID", ssid);
    preferences.putString("WF_PASS", password);
    preferences.end();  // Close the Preferences
    request->send(200, "text/plain", "WiFi credentials saved.");
  } else {
    request->send(400, "text/plain", "Invalid input.");
  }
}

void handleWifiCredentialsClear(AsyncWebServerRequest *request, Preferences &preferences) {
  preferences.begin("wifi_cred", false);
  preferences.remove("WF_SSID");
  preferences.remove("WF_PASS");
  preferences.end();
  request->send(200, "text/plain", "WiFi credentials cleared.");
}

void handleGetWifiAp(AsyncWebServerRequest *request, Preferences &preferences) {
  preferences.begin("wifi_cred", false);
  String ssid = preferences.getString("WF_SSID", "");
  preferences.end();
  request->send(200, "text/plain", ssid);
}

void handleGetPressureValue(AsyncWebServerRequest *request, int &pressureValue) {
  String pressureValueString = String(pressureValue);
  request->send(200, "text/plain", pressureValueString);
}

void handlePostOtaUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) {
    Serial.printf("OTA update request - filename: %s\n", filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
    }
  }
  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }
  if (final) {
    if (Update.end(true)) {
      Serial.printf("OTA update complete - %u bytes\n", index + len);
      request->redirect("/utils");
      //request->send(200, "text/plain", "Update Successful");
    } else {
      Update.printError(Serial);
      request->send(400, "text/plain", "Update Failed");
    }
  }
}