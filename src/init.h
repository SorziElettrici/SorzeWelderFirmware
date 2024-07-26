#ifndef INIT_H
#define INIT_H

#include <Arduino.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <secrets.h>

// Initialize serial communication
void initializeSerial(HardwareSerial &localSerial);

// Initialize LCD
void initializeLCD(LiquidCrystal_I2C &localLCD);

// Initialize WiFi
void initializeWiFi(String localWiFiSSID, String localWiFiPass, HardwareSerial &localSerial, LiquidCrystal_I2C &localLCD, bool &localWifiAPMode);

// Read WiFi credentials from the preferences
void readWifiCredential(String &localWiFiSSID, String &localWiFiPass, Preferences &localPreferences, HardwareSerial &localSerial, LiquidCrystal_I2C &localLCD);

// Read Zap preferences
void readZappPreferences(int &localMainsFrequency, int &localPreHeatHalfCycles, int &localPreHeatZappIntervalHalfCycles, int &localZappHalfCycles, Preferences &localPreferences);

// Read Fan preferences
void readFanPreferences(int &localFanMode, int &localMinFanSpeed, int &localMaxFanSpeed, Preferences &localPreferences);

#endif //INIT_H