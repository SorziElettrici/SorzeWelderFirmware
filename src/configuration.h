#pragma once

// Define the pins for the components
#define HEARTHBEAT_LED_PIN 2
#define OUTPUT_ZAPP_PIN 16
#define INPUT_ZAPP_PIN 15
#define FAN_OUTPUT_PIN 32
#define ONEWIRE_PIN 4
#define ROTARY_ENCODER_CLK 14
#define ROTARY_ENCODER_DT 27
#define ROTARY_ENCODER_SW 26

// Zapp Defaults
#define MAINS_FREQUENCY_PREF "MAINS_FREQ"
#define DEFAULT_MAINS_FREQUENCY 50
#define PREHEAT_HALFCYCLES_PREF "PH_HALFCYC"
#define DEFAULT_PREHEAT_HALFCYCLES 3
#define PREHEAT_ZAPP_INTERVAL_HALFCYCLES_PREF "PH_Z_INT_HCYC"
#define DEFAULT_PREHEAT_ZAPP_INTERVAL_HALFCYCLES 1
#define ZAPP_HALFCYCLES_PREF "Z_HALFCYC"
#define DEFAULT_ZAPP_HALFCYCLES 12
#define SAFE_ZAPP_INTERVAL 3000
#define ZAPP_DEBOUNCE_DELAY 100

// Fan Defaults
#define FAN_MODE_PREF "FAN_MODE"
#define DEFAULT_FAN_MODE 0
#define MIN_FAN_SPEED_PREF "MIN_FAN_SPEED"
#define DEFAULT_MIN_FAN_SPEED 0
#define MAX_FAN_SPEED_PREF "MAX_FAN_SPEED"
#define DEFAULT_MAX_FAN_SPEED 255
#define FAN_TEMPERATURE_THRESHOLD 30

// Temperature Defaults
#define TEMPERATURE_UPDATE_INTERVAL 1000
#define TEMPERATURE_SENSOR_RESOLUTION 12