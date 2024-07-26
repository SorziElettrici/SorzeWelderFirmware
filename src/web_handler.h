#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <Update.h>

#include "html/control_panel.h"
#include "html/fan_panel.h"
#include "html/wifi_page.h"
#include "html/ota_page.h"
#include "html/utils_page.h"
#include "html/common_html.h"
#include <zapp.h>
#include <fan.h>


//
// Webpages functions
//
void serveControlPanelPage(AsyncWebServerRequest *localRequest,
                            int localMainsFrequency,
                            int localPreHeatHalfCycles,
                            int localPreHeatZappIntervalHalfCycles,
                            int localZappHalfCycles,
                            bool localZappState);
void serveFanPanelPage(AsyncWebServerRequest *localRequest,
                        int localFanMode,
                        int localFanSpeed,
                        int localMinFanSpeed,
                        int localMaxFanSpeed,
                        float localElectronicsTemperature,
                        float localTransformerTemperature);
void serveUtilsPage(AsyncWebServerRequest *request);
void serveWifiPage(AsyncWebServerRequest *request);
void serveFirmwareUpdatePage(AsyncWebServerRequest *request);


//
// ZAPP Web functions
//
void handlePostMainsFrequency(AsyncWebServerRequest *localRequest, int &localMainsFrequency, Preferences &localPreferences);
void handlePostPreHeatHalfCycles(AsyncWebServerRequest *localRequest, int &localPreHeatHalfCycles, Preferences &localPreferences);
void handlePostPreHeatZappIntervalHalfCycles(AsyncWebServerRequest *localRequest, int &localPreHeatZappIntervalHalfCycles, Preferences &localPreferences);
void handlePostZappHalfCycles(AsyncWebServerRequest *localRequest, int &localZappHalfCycles, Preferences &localPreferences);
void handlePostResetZappSettings(AsyncWebServerRequest *localRequest, int &localMainsFrequency, int &localPreHeatHalfCycles, int &localPreHeatZappIntervalHalfCycles, int &localZappHalfCycles, Preferences &localPreferences);
void handlePostZappCommand(AsyncWebServerRequest *localRequest, unsigned long &localLastZappCommandRequestedTime, unsigned long localLastZappCommandExecutedTime);
void handleGetZappState(AsyncWebServerRequest *request, bool &localZappState);


//
// Fan  Web functions
//
void handlePostFanSpeeds(AsyncWebServerRequest *localRequest, int &localMinFanSpeed, int &localMaxFanSpeed, Preferences &localPreferences);
void handlePostFanMode(AsyncWebServerRequest *localRequest, int &localFanMode, int newFanMode, Preferences &localPreferences);
void handlePostResetFanSettings(AsyncWebServerRequest *localRequest, int &localFanMode, int &localMinFanSpeed, int &localMaxFanSpeed, Preferences &localPreferences);


//
// Command handling functions
//
void handleWifiCredentialsSave(AsyncWebServerRequest *request, Preferences &preferences);
void handleWifiCredentialsClear(AsyncWebServerRequest *request, Preferences &preferences);
void handleGetWifiAp(AsyncWebServerRequest *request, Preferences &preferences);
void handlePostOtaUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);

#endif //WEB_HANDLER_H