#include <Arduino.h>
#include <configuration.h>
#include <init.h>

#include <heartbeat.h>
#include <web_handler.h>
#include <pump.h>

#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>




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




// Declare FreeRTOS task functions and handlers
TaskHandle_t TaskHeartbeatHandle;

void TaskServer (void *pvParameters);
TaskHandle_t TaskServerHandle;
// void TaskEnableButton(void *pvParameters);
// TaskHandle_t TaskEnableButtonHandle;
void TaskPressureSensor(void *pvParameters);
TaskHandle_t TaskPressureSensorHandle;




// Wifi Server state
bool wifiAPMode = false;

// State of the pump
bool pumpState = false;
int pumpSpeed = 0;

//Enable button state
bool enableButtonState = false;

// Global pressure sensor value
int pressureSensorValue = 0;

OneWire oneWire(ONEWIRE_PIN);
DallasTemperature sensors(&oneWire);

// Initialize the server on port 80
AsyncWebServer server(80);

//
// Variables
//
String wiFiSSID = "";
String wiFiPass = "";

void setup() {
  // Set pin modes
  pinMode(PUMP_OUTPUT_PIN, OUTPUT);
  pinMode(HEARTHBEAT_LED_PIN, OUTPUT);
  pinMode(SCREEN_BUTTON_PIN, INPUT);
  pinMode(ENABLE_LED_PIN, OUTPUT);
  pinMode(ENABLE_BUTTON_PIN, INPUT);

  // Initialize serial communication
  initializeSerial(mySerial);

  // Initialize LCD
  initializeLCD(myLCD);

  // Load pump speed from preferences
  reloadPumpSpeed(pumpSpeed, pumpState, preferences);

  // Load preferences for WiFi credentials
  readWifiCredential(wiFiSSID, wiFiPass, preferences, mySerial, myLCD);

  // Initialize WiFi
  initializeWiFi(wiFiSSID, wiFiPass, mySerial, myLCD, wifiAPMode);

  // Create the heartbeat task
  xTaskCreatePinnedToCore(TaskHeartbeat, "TaskHeartbeat", 2048, NULL, 3, &TaskHeartbeatHandle, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskServer, "TaskServer", 2048, NULL, 3, &TaskServerHandle, ARDUINO_RUNNING_CORE);
  // xTaskCreatePinnedToCore(TaskEnableButton, "TaskEnableButton", 2048, NULL, 3, &TaskEnableButtonHandle, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskPressureSensor, "TaskPressureSensor", 2048, NULL, 3, &TaskPressureSensorHandle, ARDUINO_RUNNING_CORE);
}

void loop() {
  // Empty loop since all tasks are handled asynchronously
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
      request->redirect("/commandpage");
    });
  }

  // Set the routes for the command page
  server.on("/commandpage", HTTP_GET, [](AsyncWebServerRequest *request){
    serveHomePage(request, pumpState, pumpSpeed, pressureSensorValue);
  });

  server.on("/toggle", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostTogglePump(request, pumpSpeed, pumpState, preferences);
  });

  server.on("/set-speed", HTTP_POST, [&](AsyncWebServerRequest *request){
    handlePostPumpSpeed(request, pumpSpeed, pumpState, preferences);
  });

  server.on("/wifi", HTTP_GET, serveWifiPage);
  server.on("/firmwareupdate", HTTP_GET, serveFirmwareUpdatePage);
  server.on("/utils", HTTP_GET, serveUtilsPage);
  server.on("/uploadfimrware", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200);
  }, handlePostOtaUpdate);
  server.on("/save", HTTP_POST, [&](AsyncWebServerRequest *request){
    handleWifiCredentialsSave(request, preferences);
  });
  server.on("/clear-wifi", HTTP_POST, [&](AsyncWebServerRequest *request){
    handleWifiCredentialsClear(request, preferences);
  });
  server.on("/current-ap", HTTP_GET, [&](AsyncWebServerRequest *request){
    handleGetWifiAp(request, preferences);
  });
  server.on("/get-pressure-value", HTTP_GET, [&](AsyncWebServerRequest *request){
    handleGetPressureValue(request, pressureSensorValue);
  });

  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Restarting Device");
    delay(500);  // Allow time for the response to reach the client before restarting the device
    ESP.restart();
  });
  
  server.begin();

  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}


// //
// // Enable Button
// //
// void TaskEnableButton(void *pvParameters) {
//   while (1) {
//     // Read the current state of the enable button pin
//     bool currentEnableButtonState = digitalRead(ENABLE_BUTTON_PIN);

//     // Check if the button state has changed
//     if (currentEnableButtonState != enableButtonState) {
//       // Update the LCD display and serial output
//       myLCD.setCursor(0, 1);
//       myLCD.clear();
//       myLCD.print(currentEnableButtonState ? "Enabled " : "Disabled");
//       mySerial.print("Enable Button: ");
//       mySerial.println(currentEnableButtonState ? "Enabled" : "Disabled");

//       // Update the previous button state
//       enableButtonState = currentEnableButtonState;
//     }

//     // Delay before checking the button state again
//     vTaskDelay(pdMS_TO_TICKS(100));
//   }
// }


// Pressure Sensor task function to read the sensor value at a frequency of 0.2Hz
void TaskPressureSensor(void *pvParameters) {
  while (1) {
    // Read the pressure sensor value and store it in the global variable
    pressureSensorValue = analogRead(PRESSURE_SENSOR_PIN);
    // Delay for 5 seconds (0.2Hz frequency)
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}