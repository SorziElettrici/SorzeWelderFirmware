#include <pump.h>
#include <configuration.h>

void setPumpSpeed(int &speed, int &newSpeed, bool &state, Preferences &pref) {
  if (newSpeed < 0) {
    newSpeed = 0;
  } else if (newSpeed > 255) {
    newSpeed = 255;
  }

  speed = newSpeed;

  pref.begin("pump_prefs", false);
  pref.putInt("pump_speed", speed);
  pref.end();

  commandPump(speed, state);
}

void resetPumpSpeed(int &speed, bool &state, Preferences &pref) {
  speed = 0;

  pref.begin("pump_prefs", false);
  pref.putInt("pump_speed", speed);
  pref.end();

  commandPump(speed, state);
}

void reloadPumpSpeed(int &speed, bool &state, Preferences &pref) {
  pref.begin("pump_prefs", false);
  int savedPumpSpeed = pref.getInt("pump_speed", 0);
  pref.end();

  commandPump(speed, state);

  speed = savedPumpSpeed;

  commandPump(speed, state);
}

void setPumpState(int &speed, bool &state, bool &newState) {
  state = newState;

  commandPump(speed, state);
}

void togglePumpState(int &speed, bool &state) {
  bool newState = !state;
  setPumpState(speed, state, newState);
}

void commandPump(int &speed, bool &state) {
  if (state) {
    analogWrite(PUMP_OUTPUT_PIN, speed);
  } else {
    analogWrite(PUMP_OUTPUT_PIN, 0);
  }
}