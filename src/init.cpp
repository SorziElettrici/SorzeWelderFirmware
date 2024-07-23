#include <init.h>
#include <configuration.h>

void initializeSerial(HardwareSerial &localSerial) {
  localSerial.begin(250000);
  while (!localSerial) {
    delay(10);
  }
  localSerial.println("Serial communication started.");
}

void initializeLCD(LiquidCrystal_I2C &localLCD) {
  localLCD.init();
  localLCD.backlight();
  localLCD.setCursor(0, 0);
  localLCD.print("Booting...");
}

void initializeWiFi(String localWiFiSSID, String localWiFiPass, HardwareSerial &localSerial, LiquidCrystal_I2C &localLCD, bool &localWifiAPMode) {
  // Attempt to connect to the stored WiFi credentials
  WiFi.mode(WIFI_STA);
  WiFi.begin(localWiFiSSID.c_str(), localWiFiPass.c_str());
  localLCD.setCursor(0, 0);
  localLCD.clear();
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    localSerial.print(".");
    localLCD.print(".");
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  // Check if the connection was successful
  if (WiFi.status() == WL_CONNECTED) {
    // If WiFi credentials are valid, set the access point mode to false
    localWifiAPMode = false;

    localLCD.setCursor(0, 0);
    localLCD.clear();
    localLCD.print("WiFi Connected");
    localLCD.setCursor(0, 1);
    localLCD.print(WiFi.localIP());

    localSerial.println("");
    localSerial.print("Connected to ");
    localSerial.println(localWiFiSSID);
    localSerial.print("IP address: ");
    localSerial.println(WiFi.localIP());
  } else {
    // If WiFi credentials are not valid, create an access point and set the access point mode to true
    localWifiAPMode = true;

    localLCD.setCursor(0, 0);
    localLCD.clear();
    localLCD.print("WiFi AP Mode");

    localSerial.println("Failed to connect to stored WiFi credentials.");
    localSerial.println("Creating WiFi access point...");
  }
}

void readWifiCredential(String &localWiFiSSID,
                        String &localWiFiPass,
                        Preferences &localPreferences,
                        HardwareSerial &localSerial,
                        LiquidCrystal_I2C &localLCD) {
    localLCD.setCursor(0, 0);
    localLCD.clear();
    localLCD.print("Read WiFi cred");
    localLCD.setCursor(0, 1);
    localLCD.print("from prefs...");
    localSerial.println("Reading WiFi credentials from preferences...");
    localPreferences.begin("wifi_cred", true); // Open in read-only mode

    // Read existing values if they are available
    localWiFiSSID = localPreferences.getString("WF_SSID", ""); // Default to empty if not found
    localWiFiPass = localPreferences.getString("WF_PASS", "");

    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second to display the message

    // Check if either SSID or password are missing
    if (localWiFiSSID.isEmpty() || localWiFiPass.isEmpty()) {
        localLCD.setCursor(0, 0);
        localLCD.clear();
        localLCD.print("WiFi cred not");
        localLCD.setCursor(0, 1);
        localLCD.print("found in prefs");
        localSerial.println("WiFi credentials not found in preferences.");

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second to display the message

        localLCD.setCursor(0, 0);
        localLCD.clear();
        localLCD.print("Storing default");
        localLCD.setCursor(0, 1);
        localLCD.print("WiFi cred...");
        localSerial.println("Storing default WiFi credentials in preferences...");

        // Re-open preferences in read-write mode
        localPreferences.end(); // Close read-only session
        localPreferences.begin("wifi_cred", false); // Open in read-write mode
        
        // Store the defaults from secrets.h
        localPreferences.putString("WF_SSID", WIFI_SSID);
        localWiFiSSID = WIFI_SSID;
        localPreferences.putString("WF_PASS", WIFI_PASSWORD);
        localWiFiPass = WIFI_PASSWORD;

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second to display the message

        // Close the preferences and re-open in read-only mode to check if the values were stored correctly
        localPreferences.end(); // Close read-write session
        localPreferences.begin("wifi_cred", true); // Open in read-only mode
        
        if (localPreferences.getString("WF_SSID", "") == WIFI_SSID && localPreferences.getString("WF_PASS", "") == WIFI_PASSWORD) {
            localLCD.setCursor(0, 0);
            localLCD.clear();
            localLCD.print("Default WiFi");
            localLCD.setCursor(0, 1);
            localLCD.print("cred stored");
            localSerial.println("Default WiFi credentials stored successfully.");
        } else {
            localLCD.setCursor(0, 0);
            localLCD.clear();
            localLCD.print("Failed to store");
            localLCD.setCursor(0, 1);
            localLCD.print("WiFi cred");
            localSerial.println("Failed to store default WiFi credentials.");

        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second to display the message
    } else {
        localLCD.setCursor(0, 0);
        localLCD.clear();
        localLCD.print("WiFi cred found");
        localLCD.setCursor(0, 1);
        localLCD.print("in prefs");
        localSerial.println("WiFi credentials found in preferences.");

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second to display the message
    }

    localPreferences.end(); // Always close the preferences
}