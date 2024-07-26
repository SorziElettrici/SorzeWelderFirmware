#include <fan.h>
#include <configuration.h>
#include <PID_v1.h>

void setFanSpeeds(int &localMinFanSpeed, int &localMaxFanSpeed, int newMinFanSpeed, int newMaxFanSpeed, Preferences &localPreferences) {
  localPreferences.begin("fan_prefs", false);
  localPreferences.putInt(MIN_FAN_SPEED_PREF, newMinFanSpeed);
  localPreferences.putInt(MAX_FAN_SPEED_PREF, newMaxFanSpeed);
  localPreferences.end();

  localPreferences.begin("fan_prefs", true);
  int savedMinFanSpeed = localPreferences.getInt(MIN_FAN_SPEED_PREF, 0);
  int savedMaxFanSpeed = localPreferences.getInt(MAX_FAN_SPEED_PREF, 0);
  localPreferences.end();

  if (savedMinFanSpeed == newMinFanSpeed && savedMaxFanSpeed == newMaxFanSpeed)
  {
    localMinFanSpeed = newMinFanSpeed;
    localMaxFanSpeed = newMaxFanSpeed;
  }
}

void setFanMode(int &localFanMode, int newFanMode, Preferences &localPreferences) {
  localPreferences.begin("fan_prefs", false);
  localPreferences.putInt(FAN_MODE_PREF, newFanMode);
  localPreferences.end();

  localPreferences.begin("fan_prefs", true);
  int savedFanMode = localPreferences.getInt(FAN_MODE_PREF, 0);
  localPreferences.end();

  if (savedFanMode == newFanMode)
  {
    localFanMode = newFanMode;
  }
}


void TaskFan(void *pvParameters) {
  void **localParameters = (void **)pvParameters; // Array of pointers to parameters

  // Casting of parameters to the correct type
  int &fanMode = *(int *)localParameters[0];
  int &currentFanSpeed = *(int  *)localParameters[1];
  int &minFanSpeed = *(int *)localParameters[2];
  int &maxFanSpeed = *(int *)localParameters[3];
  float &electronicsTemperature = *(float *)localParameters[4];
  float &transformerTemperature = *(float *)localParameters[5];

  double maxTemperature = 0.0;
  double newFanSpeed = 0.0;

  // PID parameters
  const double Kp = 2.0;  // Proportional gain
  const double Ki = 0.1;  // Integral gain
  const double Kd = 0.05; // Derivative gain
  double setpoint = FAN_TEMPERATURE_THRESHOLD; // Setpoint

  // PID object creation
  PID electronicsTemperaturePID(&maxTemperature, &newFanSpeed, &setpoint, Kp, Ki, Kd, DIRECT);
  electronicsTemperaturePID.SetMode(AUTOMATIC);
  electronicsTemperaturePID.SetOutputLimits(minFanSpeed, maxFanSpeed);
  
  while (1) {

    if (fanMode == 0) {
      maxTemperature = max(electronicsTemperature, transformerTemperature);
      electronicsTemperaturePID.Compute(); // Calculate PID output
      currentFanSpeed = (int)newFanSpeed;
    } else if (fanMode == 1) {
      currentFanSpeed = maxFanSpeed;
    } else {
      currentFanSpeed = 0;
    }

    analogWrite(FAN_OUTPUT_PIN, currentFanSpeed);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}