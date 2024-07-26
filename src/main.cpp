#include <Arduino.h>
#include <configuration.h>
#include <init.h>

#include <heartbeat.h>
#include <web_handler.h>
#include <zapp.h>
#include <fan.h>
#include <temperature.h>

#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Encoder.h>




// FreeRTOS Configuration: Set the running core depending on the configuration
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif




// Create the preferences object to store settings
Preferences preferences;

// Create the lcd object address 0x27 and 16 columns x 2 rows 
LiquidCrystal_I2C myLCD (0x27, 16,2);

// Create the serial object for the ESP32 hardware serial port 0
HardwareSerial mySerial(0);

// Create the OneWire object
OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);

// Create the ESP32Encoder object
ESP32Encoder encoder;


// Declare FreeRTOS task functions and handlers
void TaskServer (void *pvParameters);
TaskHandle_t TaskServerHandle;
TaskHandle_t TaskHeartbeatHandle;
TaskHandle_t TaskReadZappHandle;
TaskHandle_t TaskExecuteZappHandle;
TaskHandle_t TaskFanHandle;
TaskHandle_t TaskTemperatureHandle;




// Wifi Server state
bool wifiAPMode = false;

// Mains frequency
int mainsFrequency = 0;

// PreHeat HalfCycles
int preHeatHalfCycles = 0;

// PreHeat Zapp Interval
int preHeatZappIntervalHalfCycles = 0;

// Zapp HalfCycles
int zappHalfCycles = 0;

// Zapp State
bool zappState = false;

// Zapp Command Requested Time
unsigned long lastZappCommandRequestedTime = 0;

// Last Zapp Command Executed Time
unsigned long lastZappCommandExecutedTime = 0;

// FAN Mode
int fanMode = 0; // -1: OFF, 0: AUTO, 1: ON

// Current Fan Speed
int currentFanSpeed = 0;

// Minimum Fan Speed
int minFanSpeed = 0;

// Maximum Fan Speed
int maxFanSpeed = 255;

// Electronics Temperature
float electronicsTemperature = 0.0;

// Transformer Temperature
float transformerTemperature = 0.0;

// Initialize the server on port 80
AsyncWebServer server(80);

volatile int lastEncoded = 0;
volatile int lastEncoderSwitchState = LOW;

//
// Variables
//
String wiFiSSID = "";
String wiFiPass = "";

void setup() {
  // Set pin modes
  pinMode(HEARTHBEAT_LED_PIN, OUTPUT);
  pinMode(OUTPUT_ZAPP_PIN, OUTPUT);
  pinMode(INPUT_ZAPP_PIN, INPUT_PULLUP);
  pinMode(FAN_OUTPUT_PIN, OUTPUT);

  // Initialize serial communication
  initializeSerial(mySerial);

  // Initialize LCD
  initializeLCD(myLCD);

  // Load Zapp preferences
  readZappPreferences(mainsFrequency, preHeatHalfCycles, preHeatZappIntervalHalfCycles, zappHalfCycles, preferences);

  // Load Fan preferences
  readFanPreferences(fanMode, minFanSpeed, maxFanSpeed, preferences);

  // Load preferences for WiFi credentials
  readWifiCredential(wiFiSSID, wiFiPass, preferences, mySerial, myLCD);

  // Initialize WiFi
  initializeWiFi(wiFiSSID, wiFiPass, mySerial, myLCD, wifiAPMode);

 void *fanParameters[6] = {&fanMode, &currentFanSpeed, &minFanSpeed, &maxFanSpeed, &electronicsTemperature, &transformerTemperature}; 
 void *temperatureParameters[3] = {&electronicsTemperature, &transformerTemperature, &sensors};
 void *readZappParameters[3] = {&lastZappCommandRequestedTime, &lastZappCommandExecutedTime, &mySerial};
 void *executeZappParameters[9] = {&mainsFrequency, &preHeatHalfCycles, &preHeatZappIntervalHalfCycles, &zappHalfCycles, &zappState, &mySerial, &myLCD, &lastZappCommandRequestedTime, &lastZappCommandExecutedTime};

  // Create the heartbeat task
  xTaskCreatePinnedToCore(TaskHeartbeat, "TaskHeartbeat", 2048, NULL, 3, &TaskHeartbeatHandle, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskServer, "TaskServer", 2048, NULL, 3, &TaskServerHandle, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskReadZapp, "TaskReadZapp", 2048, readZappParameters, 3, &TaskReadZappHandle, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskExecuteZapp, "TaskExecuteZapp", 2048, executeZappParameters, 3, &TaskExecuteZappHandle, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskFan, "TaskFan", 2048, fanParameters, 3, &TaskFanHandle, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskTemperature, "TaskTemperature", 2048, temperatureParameters, 3, &TaskTemperatureHandle, ARDUINO_RUNNING_CORE);

  // Inizializza l'encoder
  encoder.attachHalfQuad(ROTARY_ENCODER_DT, ROTARY_ENCODER_CLK);
  encoder.setCount(0);

  // Configura il pin del pulsante come input con pull-up
  pinMode(ROTARY_ENCODER_SW, INPUT);
}

void loop() {
  // Leggi la posizione dell'encoder
  int encoded = encoder.getCount();

  // Controlla se c'è stata una rotazione
  if (encoded != lastEncoded) {
    mySerial.print("Rotazione: ");
    mySerial.println(encoded);
    lastEncoded = encoded;
  }

  // Leggi lo stato del pulsante
  int encoderSwitchState = digitalRead(ROTARY_ENCODER_SW);

  // Controlla se il pulsante è stato premuto (transizione da HIGH a LOW)
  if (encoderSwitchState == LOW && lastEncoderSwitchState == HIGH) {
    mySerial.println("Pulsante premuto!");
  }

  // Aggiorna lo stato precedente del pulsante
  lastEncoderSwitchState = encoderSwitchState;

  // Piccolo ritardo per evitare letture troppo frequenti (puoi regolare)
  delay(10);
}

// FREE RTOS TASKS
void TaskServer(void *pvParameters) {
  // Set the homepage route based on the WiFi mode
  if (wifiAPMode) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->redirect("/wifi");
    });
  } else {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->redirect("/controlpanel");
    });
  }

  // Set the routes for the control panel
  server.on("/controlpanel", HTTP_GET, [](AsyncWebServerRequest *request){
    serveControlPanelPage(request, mainsFrequency, preHeatHalfCycles, preHeatZappIntervalHalfCycles, zappHalfCycles, zappState);
  });

  server.on("/zapp-command", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostZappCommand(request, lastZappCommandRequestedTime, lastZappCommandExecutedTime);
  });

  server.on("/set-mains-frequency", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostMainsFrequency(request, mainsFrequency, preferences);
  });

  server.on("/set-preheat-halfcycles", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostPreHeatHalfCycles(request, preHeatHalfCycles, preferences);
  });

  server.on("/set-preheat-zapp-interval-halfcycles", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostPreHeatZappIntervalHalfCycles(request, preHeatZappIntervalHalfCycles, preferences);
  });

  server.on("/set-zapp-halfcycles", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostZappHalfCycles(request, zappHalfCycles, preferences);
  });

  server.on("/reset-zapp-settings", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostResetZappSettings(request, mainsFrequency, preHeatHalfCycles, preHeatZappIntervalHalfCycles, zappHalfCycles, preferences);
  });

  server.on("/get-zapp-state", HTTP_GET, [&](AsyncWebServerRequest *request){
    handleGetZappState(request, zappState);
  });


  // Set the routes for the fan panel page
  server.on("/fanpanel", HTTP_GET, [](AsyncWebServerRequest *request){
    serveFanPanelPage(request, fanMode, currentFanSpeed, minFanSpeed, maxFanSpeed, electronicsTemperature, transformerTemperature);
  });

  server.on("/set-fan-speeds", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostFanSpeeds(request, minFanSpeed, maxFanSpeed, preferences);
  });

  server.on("/set-fan-off", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostFanMode(request, fanMode, -1, preferences);
  });

  server.on("/set-fan-auto", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostFanMode(request, fanMode, 0, preferences);
  });

  server.on("/set-fan-on", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostFanMode(request, fanMode, 1, preferences);
  });

  server.on("/reset-fan-settings", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostResetFanSettings(request, fanMode, minFanSpeed, maxFanSpeed, preferences);
  });


  // Set the routes for the wifi page
  server.on("/wifi", HTTP_GET, serveWifiPage);
  server.on("/save", HTTP_POST, [&](AsyncWebServerRequest *request){
    handleWifiCredentialsSave(request, preferences);
  });
  server.on("/clear-wifi", HTTP_POST, [&](AsyncWebServerRequest *request){
    handleWifiCredentialsClear(request, preferences);
  });
  server.on("/current-ap", HTTP_GET, [&](AsyncWebServerRequest *request){
    handleGetWifiAp(request, preferences);
  });

  // Set the routes for the utils page
  server.on("/utils", HTTP_GET, serveUtilsPage);
  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Restarting Device");
    delay(500);  // Allow time for the response to reach the client before restarting the device
    ESP.restart();
  });

  // Set the routes for the firmware update page
  server.on("/firmwareupdate", HTTP_GET, serveFirmwareUpdatePage);
  server.on("/uploadfimrware", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200);
  }, handlePostOtaUpdate);
  
  server.begin();

  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
