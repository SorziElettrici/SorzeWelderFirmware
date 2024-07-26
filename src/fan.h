#ifndef FAN_H
#define FAN_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>

void setFanSpeeds(int &localMinFanSpeed, int &localMaxFanSpeed, int newMinFanSpeed, int newMaxFanSpeed, Preferences &localPreferences);
void setFanMode(int &localFanMode, int newFanMode, Preferences &localPreferences);

void TaskFan(void *pvParameters);

#endif //FAN_H