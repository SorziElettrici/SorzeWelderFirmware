#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>
#include <Preferences.h>

void setPumpSpeed(int &speed, int &newSpeed, bool &state, Preferences &pref);
void resetPumpSpeed(int &speed, bool &state, Preferences &pref);
void reloadPumpSpeed(int &speed, bool &state, Preferences &pref);
void setPumpState(int &speed, bool &state, bool &newState);
void togglePumpState(int &speed, bool &state);
void commandPump(int &speed, bool &state);

#endif //PUMP_H