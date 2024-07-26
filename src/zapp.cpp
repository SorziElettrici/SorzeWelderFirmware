#include <zapp.h>
#include <LiquidCrystal_I2C.h>
#include <configuration.h>

void setMainsFrequency(int &localMainsFrequency, int newMainsFrequency, Preferences &localPreferences) {
  localPreferences.begin("zapp_prefs", false);
  localPreferences.putInt(MAINS_FREQUENCY_PREF, newMainsFrequency);
  localPreferences.end();

  localPreferences.begin("zapp_prefs", true);
  int savedMainsFrequency = localPreferences.getInt(MAINS_FREQUENCY_PREF, 0);
  localPreferences.end();

  if (savedMainsFrequency == newMainsFrequency)
  {
    localMainsFrequency = newMainsFrequency;
  }
}

void setPreHeatHalfCycles(int &localPreHeatHalfCycles, int newPreHeatHalfCycles, Preferences &localPreferences) {
  localPreferences.begin("zapp_prefs", false);
  localPreferences.putInt(PREHEAT_HALFCYCLES_PREF, newPreHeatHalfCycles);
  localPreferences.end();

  localPreferences.begin("zapp_prefs", true);
  int savedPreHeatHalfCycles = localPreferences.getInt(PREHEAT_HALFCYCLES_PREF, 0);
  localPreferences.end();

  if (savedPreHeatHalfCycles == newPreHeatHalfCycles)
  {
    localPreHeatHalfCycles = newPreHeatHalfCycles;
  }
}

void setPreHeatZappIntervalHalfCycles(int &localPreHeatZappIntervalHalfCycles, int newPreHeatZappIntervalHalfCycles, Preferences &localPreferences) {
  localPreferences.begin("zapp_prefs", false);
  localPreferences.putInt(PREHEAT_ZAPP_INTERVAL_HALFCYCLES_PREF, newPreHeatZappIntervalHalfCycles);
  localPreferences.end();

  localPreferences.begin("zapp_prefs", true);
  int savedPreHeatZappIntervalHalfCycles = localPreferences.getInt(PREHEAT_ZAPP_INTERVAL_HALFCYCLES_PREF, 0);
  localPreferences.end();

  if (savedPreHeatZappIntervalHalfCycles == newPreHeatZappIntervalHalfCycles)
  {
    localPreHeatZappIntervalHalfCycles = newPreHeatZappIntervalHalfCycles;
  }
}

void setZappHalfCycles(int &localZappHalfCycles, int newZappHalfCycles, Preferences &localPreferences) {
  localPreferences.begin("zapp_prefs", false);
  localPreferences.putInt(ZAPP_HALFCYCLES_PREF, newZappHalfCycles);
  localPreferences.end();

  localPreferences.begin("zapp_prefs", true);
  int savedZappHalfCycles = localPreferences.getInt(ZAPP_HALFCYCLES_PREF, 0);
  localPreferences.end();

  if (savedZappHalfCycles == newZappHalfCycles)
  {
    localZappHalfCycles = newZappHalfCycles;
  }
}

void zappCommand(unsigned long &localLastZappCommandRequestedTime, unsigned long localLastZappCommandExecutedTime)
{
  if ((localLastZappCommandRequestedTime <= localLastZappCommandExecutedTime) && ((xTaskGetTickCount() - localLastZappCommandExecutedTime) > SAFE_ZAPP_INTERVAL)) {
    localLastZappCommandRequestedTime = xTaskGetTickCount();
  }
}

void TaskReadZapp(void *pvParameters) {
  void **localParameters = (void **)pvParameters; // Array of pointers to parameters

  // Casting of parameters to the correct type
  unsigned long &lastZappCommandRequestedTime = *(unsigned long *)localParameters[0];
  unsigned long &lastZappCommandExecutedTime = *(unsigned long *)localParameters[1];
  HardwareSerial &localSerial = *(HardwareSerial *)localParameters[2];

  // Debouncing state variables
  bool buttonState = HIGH;         // Current button state (assuming normally HIGH due to pull-up)
  bool lastButtonState = HIGH;     // Previous button state
  unsigned long lastDebounceTime = 0; // Last time button state changed

  while (1) {
    // Read the button state
    int reading = digitalRead(INPUT_ZAPP_PIN);

    // Check if button state changed
    if (reading != lastButtonState) {
      lastDebounceTime = xTaskGetTickCount();  // Reset the debouncing timer
    }

    // Check if the debounce time has elapsed
    if ((xTaskGetTickCount() - lastDebounceTime) > ZAPP_DEBOUNCE_DELAY) {

      // If the button state has truly changed
      if (reading != buttonState) {
        buttonState = reading;

        // Check if button is pressed (LOW due to pull-up)
        if (buttonState == LOW) {
          if ((lastZappCommandRequestedTime <= lastZappCommandExecutedTime) && ((xTaskGetTickCount() - lastZappCommandExecutedTime) > SAFE_ZAPP_INTERVAL)) {
            lastZappCommandRequestedTime = xTaskGetTickCount();
          }
        }
      }
    }

    lastButtonState = reading;
    vTaskDelay(pdMS_TO_TICKS(10)); // Short delay for responsiveness
  }
}

void TaskExecuteZapp(void *pvParameters) {
  void **localParameters = (void **)pvParameters; // Array of pointers to parameters

  // Casting of parameters to the correct type
  int &localMainsFrequency = *(int *)localParameters[0];
  int &localPreHeatHalfCycles = *(int *)localParameters[1];
  int &localPreHeatZappIntervalHalfCycles = *(int *)localParameters[2];
  int &localZappHalfCycles = *(int *)localParameters[3];
  bool &localZappState = *(bool *)localParameters[4];
  HardwareSerial &localSerial = *(HardwareSerial *)localParameters[5];
  LiquidCrystal_I2C &localLCD = *(LiquidCrystal_I2C *)localParameters[6];
  unsigned long &lastZappCommandRequestedTime = *(unsigned long *)localParameters[7];
  unsigned long &lastZappCommandExecutedTime = *(unsigned long *)localParameters[8];

  while (1) {
    if (lastZappCommandRequestedTime > lastZappCommandExecutedTime) {
      localZappState = true;
      localLCD.clear();
      localLCD.setCursor(0, 0);
      localLCD.print("ZAPP!!!");
      localSerial.println("ZAPP!!!");

      // PreHeat Pulse
      digitalWrite(OUTPUT_ZAPP_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(localPreHeatHalfCycles * 1000 / (localMainsFrequency * 2)));
      digitalWrite(OUTPUT_ZAPP_PIN, LOW);

      // Interval between PreHeat and Zapp
      vTaskDelay(pdMS_TO_TICKS(localPreHeatZappIntervalHalfCycles * 1000 / (localMainsFrequency * 2)));

      // ZAPP!!!
      digitalWrite(OUTPUT_ZAPP_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(localZappHalfCycles * 1000 / (localMainsFrequency * 2)));
      digitalWrite(OUTPUT_ZAPP_PIN, LOW);

      localZappState = false;
      lastZappCommandExecutedTime = xTaskGetTickCount();
      localLCD.setCursor(0, 0);
      localLCD.clear();
    }

    vTaskDelay(pdMS_TO_TICKS(10)); // Short delay for responsiveness
  }
}