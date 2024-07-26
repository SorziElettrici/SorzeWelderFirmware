#ifndef ZAPP_H
#define ZAPP_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>

void setMainsFrequency(int &localMainsFrequency, int newMainsFrequency, Preferences &localPreferences);
void setPreHeatHalfCycles(int &localPreHeatHalfCycles, int newPreHeatHalfCycles, Preferences &localPreferences);
void setPreHeatZappIntervalHalfCycles(int &localPreHeatZappInterval, int newPreHeatZappIntervalHalfCycles, Preferences &localPreferences);
void setZappHalfCycles(int &localZappHalfCycles, int newZappHalfCycles, Preferences &localPreferences);
void zappCommand(unsigned long &localLastZappCommandRequestedTime, unsigned long localLastZappCommandExecutedTime);
void TaskReadZapp(void *pvParameters);
void TaskExecuteZapp(void *pvParameters);

#endif //ZAPP_H