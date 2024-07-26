#include <web_handler.h>
#include <configuration.h>


//
// Webpages functions
//
void serveControlPanelPage(AsyncWebServerRequest *localRequest,
                            int localMainsFrequency,
                            int localPreHeatHalfCycles,
                            int localPreHeatZappIntervalHalfCycles,
                            int localZappHalfCycles,
                            bool localZappState) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m%", HTML_MENU);
  html.replace("%b%", HTML_CONTROL_PANEL);
  html.replace("%t%", "Control panel");

  html.replace("%zs%", String(localZappState ? "ON" : "OFF"));
  html.replace("%fv%", String(localMainsFrequency));
  if(localMainsFrequency == 0) {
    html.replace("%hcd%", "not available");
  } else {
    html.replace("%hcd%", String(1000/localMainsFrequency/2));
  }
  html.replace("%phhc%", String(localPreHeatHalfCycles));
  if(localMainsFrequency == 0) {
    html.replace("%phd%", "not available");
  } else {
    html.replace("%phd%", String(1000/localMainsFrequency/2*localPreHeatHalfCycles));
  }
  html.replace("%phzhc%", String(localPreHeatZappIntervalHalfCycles));
  if(localMainsFrequency == 0) {
    html.replace("%phzid%", "not available");
  } else {
    html.replace("%phzid%", String(1000/localMainsFrequency/2*localPreHeatZappIntervalHalfCycles));
  }
  html.replace("%zhc%", String(localZappHalfCycles));
  if(localMainsFrequency == 0) {
    html.replace("%zd%", "not available");
  } else {
    html.replace("%zd%", String(1000/localMainsFrequency/2*localZappHalfCycles));
  }
  localRequest->send(200, "text/html", html);
}

void serveFanPanelPage(AsyncWebServerRequest *localRequest,
                        int localFanMode,
                        int localFanSpeed,
                        int localMinFanSpeed,
                        int localMaxFanSpeed,
                        float localElectronicsTemperature,
                        float localTransformerTemperature) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m%", HTML_MENU);
  html.replace("%b%", HTML_FAN_PANEL);
  html.replace("%t%", "Fan panel");

  String localFanModeString = "";
  if (localFanMode == -1) {
    localFanModeString = "OFF";
  } else if (localFanMode == 0) {
    localFanModeString = "AUTO";
  } else if (localFanMode == 1) {
    localFanModeString = "ON";
  }

  int localMinFanSpeedPercent = (int)localMinFanSpeed / 255.0 * 100.0;
  int localMaxFanSpeedPercent = (int)localMaxFanSpeed / 255.0 * 100.0;
  int localFanSpeedPercent = (int)localFanSpeed / 255.0 * 100.0;

  html.replace("%et%", String(localElectronicsTemperature));
  html.replace("%tt%", String(localTransformerTemperature));
  html.replace("%cfm%", localFanModeString);
  html.replace("%mifs%", String(localMinFanSpeedPercent));
  html.replace("%mafs%", String(localMaxFanSpeedPercent));
  html.replace("%cfs%", String(localFanSpeedPercent));

  localRequest->send(200, "text/html", html);
}

void serveUtilsPage(AsyncWebServerRequest *request) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m%", HTML_MENU);
  html.replace("%b%", HTML_UTILSPAGE);
  html.replace("%t%", "Utilities");
  request->send(200, "text/html", html);
}

void serveWifiPage(AsyncWebServerRequest *request) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m%", HTML_MENU);
  html.replace("%b%", HTML_WIFIPAGE);
  html.replace("%t%", "Configure WiFi");
  request->send(200, "text/html", html);
}

void serveFirmwareUpdatePage(AsyncWebServerRequest *request) {
  String html = String(HTML_STRUCTURE);
  html.replace("%m%", HTML_MENU);
  html.replace("%b%", HTML_OTAPAGE);
  html.replace("%t%", "Firmware Update");
  request->send(200, "text/html", html);
}


//
// ZAPP Web functions
//
void handlePostMainsFrequency(AsyncWebServerRequest *localRequest, int &localMainsFrequency, Preferences &localPreferences) {
  int newMainsFrequency = localRequest->arg("mains-frequency").toInt();

  if (newMainsFrequency < 50) {
    newMainsFrequency = 50;
  } else if (newMainsFrequency > 60) {
    newMainsFrequency = 60;
  }

  setMainsFrequency(localMainsFrequency, newMainsFrequency, localPreferences);
  localRequest->send(200, "text/plain", "Mains Frequency saved.");
}

void handlePostPreHeatHalfCycles(AsyncWebServerRequest *localRequest, int &localPreHeatHalfCycles, Preferences &localPreferences) {
  int newPreHeatHalfCycles = localRequest->arg("preheat-halfcycles").toInt();

  if (newPreHeatHalfCycles < 0) {
    newPreHeatHalfCycles = 0;
  } else if (newPreHeatHalfCycles > 10) {
    newPreHeatHalfCycles = 10;
  }

  setPreHeatHalfCycles(localPreHeatHalfCycles, newPreHeatHalfCycles, localPreferences);
  localRequest->send(200, "text/plain", "PreHeat HalfCycles saved.");
}

void handlePostPreHeatZappIntervalHalfCycles(AsyncWebServerRequest *localRequest, int &localPreHeatZappIntervalHalfCycles, Preferences &localPreferences) {
  int newPreHeatZappIntervalHalfCycles = localRequest->arg("preheat-zapp-interval-halfcycles").toInt();

  if (newPreHeatZappIntervalHalfCycles < 0) {
    newPreHeatZappIntervalHalfCycles = 0;
  } else if (newPreHeatZappIntervalHalfCycles > 20) {
    newPreHeatZappIntervalHalfCycles = 20;
  }

  setPreHeatZappIntervalHalfCycles(localPreHeatZappIntervalHalfCycles, newPreHeatZappIntervalHalfCycles, localPreferences);
  localRequest->send(200, "text/plain", "PreHeat Zapp Interval HalfCycles saved.");
}

void handlePostZappHalfCycles(AsyncWebServerRequest *localRequest, int &localZappHalfCycles, Preferences &localPreferences) {
  int newZappHalfCycles = localRequest->arg("zapp-halfcycles").toInt();

  if (newZappHalfCycles < 0) {
    newZappHalfCycles = 0;
  } else if (newZappHalfCycles > 20) {
    newZappHalfCycles = 20;
  }

  setZappHalfCycles(localZappHalfCycles, newZappHalfCycles, localPreferences);
  localRequest->send(200, "text/plain", "Zapp HalfCycle saved.");
}

void handlePostResetZappSettings(AsyncWebServerRequest *localRequest, int &localMainsFrequency, int &localPreHeatHalfCycles, int &localPreHeatZappIntervalHalfCycles, int &localZappHalfCycles, Preferences &localPreferences) {
  setMainsFrequency(localMainsFrequency, DEFAULT_MAINS_FREQUENCY , localPreferences);
  setPreHeatHalfCycles(localPreHeatHalfCycles, DEFAULT_PREHEAT_HALFCYCLES , localPreferences);
  setPreHeatZappIntervalHalfCycles(localPreHeatZappIntervalHalfCycles, DEFAULT_PREHEAT_ZAPP_INTERVAL_HALFCYCLES , localPreferences);
  setZappHalfCycles(localZappHalfCycles, DEFAULT_ZAPP_HALFCYCLES, localPreferences);
  localRequest->send(200, "text/plain", "Zapp Settings reset.");
}

void handlePostZappCommand(AsyncWebServerRequest *localRequest, unsigned long &localLastZappCommandRequestedTime, unsigned long localLastZappCommandExecutedTime) {
  zappCommand(localLastZappCommandRequestedTime, localLastZappCommandExecutedTime);
  localRequest->send(200, "text/plain", "Zapped!");
}

void handleGetZappState(AsyncWebServerRequest *request, bool &localZappState) {
  String localZappStateString = "";
  if (localZappState) {
    localZappStateString = "ON";
  } else {
    localZappStateString = "OFF";
  }
  request->send(200, "text/plain", localZappStateString);
}


//
// Fan Web functions
//
void handlePostFanSpeeds(AsyncWebServerRequest *localRequest, int &localMinFanSpeed, int &localMaxFanSpeed, Preferences &localPreferences) {
  int newMinFanSpeed = localRequest->arg("min-fan-speed").toInt() / 100.0 * 255.0;
  int newMaxFanSpeed = localRequest->arg("max-fan-speed").toInt() / 100.0 * 255.0;

  if (newMinFanSpeed < 0) {
    newMinFanSpeed = 0;
  } else if (newMinFanSpeed > 255) {
    newMinFanSpeed = 255;
  }

  if (newMaxFanSpeed < 0) {
    newMaxFanSpeed = 0;
  } else if (newMaxFanSpeed > 255) {
    newMaxFanSpeed = 255;
  }

  setFanSpeeds(localMinFanSpeed, localMaxFanSpeed, newMinFanSpeed, newMaxFanSpeed, localPreferences);
  localRequest->send(200, "text/plain", "Fan Speeds saved.");
}
void handlePostFanMode(AsyncWebServerRequest *localRequest, int &localFanMode, int newFanMode, Preferences &localPreferences) {
  setFanMode(localFanMode, newFanMode, localPreferences);
  localRequest->send(200, "text/plain", "Fan Mode saved.");
}
void handlePostResetFanSettings(AsyncWebServerRequest *localRequest, int &localFanMode, int &localMinFanSpeed, int &localMaxFanSpeed, Preferences &localPreferences) {
  setFanMode(localFanMode, DEFAULT_FAN_MODE, localPreferences);
  setFanSpeeds(localMinFanSpeed, localMaxFanSpeed, DEFAULT_MIN_FAN_SPEED, DEFAULT_MAX_FAN_SPEED, localPreferences);
  localRequest->send(200, "text/plain", "Fan Settings reset.");
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