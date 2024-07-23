#include <Arduino.h>
#include <configuration.h>

#include <heartbeat.h>
#include <networking.h>
#include <web_handler.h>
#include <pump.h>

#include <WiFi.h>
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

Preferences preferences;

// Create the lcd object address 0x27 and 16 columns x 2 rows 
LiquidCrystal_I2C lcd (0x27, 16,2); 

// Declare FreeRTOS task functions and handlers
TaskHandle_t TaskHeartbeatHandle;

void TaskServer (void *pvParameters);
TaskHandle_t TaskServerHandle;
// void TaskEnableButton(void *pvParameters);
// TaskHandle_t TaskEnableButtonHandle;
void TaskPressureSensor(void *pvParameters);
TaskHandle_t TaskPressureSensorHandle;

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
String wf_ssid = "";
String wf_pass = "";

void setup() {
  // Set pin modes
  pinMode(PUMP_OUTPUT_PIN, OUTPUT);
  pinMode(HEARTHBEAT_LED_PIN, OUTPUT);
  pinMode(SCREEN_BUTTON_PIN, INPUT);
  pinMode(ENABLE_LED_PIN, OUTPUT);
  pinMode(ENABLE_BUTTON_PIN, INPUT);

  // Initialize serial communication
  Serial.begin(9600);
  Serial.println();

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Booting...");

  // Load pump speed from preferences
  reloadPumpSpeed(pumpSpeed, pumpState, preferences);

  // Load preferences for WiFi credentials
  readWifiCredential(wf_ssid, wf_pass, preferences);

  // Attempt to connect to the stored WiFi credentials
  WiFi.mode(WIFI_STA);
  WiFi.begin(wf_ssid.c_str(), wf_pass.c_str());
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(500);
  }

  // Check if the connection was successful
  if (WiFi.status() == WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.clear();
    Serial.println("");
    Serial.print("Connected to ");
    lcd.print("WiFi Connected");
    Serial.println(wf_ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());


    // Start the server
    server.begin();

    // Create the heartbeat task
    xTaskCreatePinnedToCore(TaskHeartbeat, "TaskHeartbeat", 2048, NULL, 3, &TaskHeartbeatHandle, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(TaskServer, "TaskServer", 2048, NULL, 3, &TaskServerHandle, ARDUINO_RUNNING_CORE);
    // xTaskCreatePinnedToCore(TaskEnableButton, "TaskEnableButton", 2048, NULL, 3, &TaskEnableButtonHandle, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(TaskPressureSensor, "TaskPressureSensor", 2048, NULL, 3, &TaskPressureSensorHandle, ARDUINO_RUNNING_CORE);

    return;
  }

  // If WiFi credentials are not valid, create an access point
  Serial.println("Failed to connect to stored WiFi credentials.");
  Serial.println("Creating WiFi access point...");
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("WiFi AP Mode");

  WiFi.mode(WIFI_AP);
  WiFi.softAP("SorzeWelder", "password");

  // Set up server routes for WiFi configuration
  server.on("/", HTTP_GET, serveWifiPage);

  server.on("/save", HTTP_POST, [&](AsyncWebServerRequest *request){
    handleWifiCredentialsSave(request, preferences);
  });

  server.on("/clear-wifi", HTTP_POST, [&](AsyncWebServerRequest *request){
    handleWifiCredentialsClear(request, preferences);
  });

  server.on("/current-ap", HTTP_GET, [&](AsyncWebServerRequest *request){
    handleGetWifiAp(request, preferences);
  });

  
  // Start the server
  server.begin();

  // Create the heartbeat task
  xTaskCreatePinnedToCore(TaskHeartbeat, "TaskHeartbeat", 2048, NULL, 3, &TaskHeartbeatHandle, ARDUINO_RUNNING_CORE);
}

void loop() {
  // Empty loop since all tasks are handled asynchronously
}

void TaskServer(void *pvParameters) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
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
//       lcd.setCursor(0, 1);
//       lcd.clear();
//       lcd.print(currentEnableButtonState ? "Enabled " : "Disabled");
//       Serial.print("Enable Button: ");
//       Serial.println(currentEnableButtonState ? "Enabled" : "Disabled");

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