#ifndef NETWORKING_H
#define NETWORKING_H

#include <Arduino.h>
#include <Preferences.h>
#include <configuration.h>
#include <secrets.h>

void readWifiCredential(String &wf_ssid, String &wf_pass, Preferences &pref);

#endif //NETWORKING_H