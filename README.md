# SorzeWelderFirmware

## Description
This is the firmware for the Sorze Welder V1.
Sorze Welder is a DIY spot welder that uses a microwave oven transformer (MOT) as
the power source, a 40A SSR to control the power output and an ESP32 to control the
SSR and handle the user interface.

## Features
- 16x2 LCD display
- Rotary encoder for user input
- Pulse input from button or foot pedal
- 40A SSR control
- Fan control
- WiFi connectivity to set the weld settings and other parameters

## If you want to use this firmware
Our actual hardware is a custom board based on ESP32-WROOM-32D, but you can use any ESP32 board.
The firmware is written in C++ using the Arduino framework, programmed using Visual Studio Code and PlatformIO.
You can get all the details on the pins used in the firmware in the `src/configuration.h` file.
WiFi settings can be set in the `src/secrets.h` file, but pay attention not to commit this file to a
public repository if you're editing it with your local WiFi settings and credentials.


## Status of the project
The project is still in development, but the firmware is mostly functional.
We're still working on the LCD interface but all the settings can be already
set using the wifi/web interface.
Fan auto mode is also still in development, the PID control is already implemented but
still not properly tuned and not working as expected.
Probably we'll also implement triple pulse mode even if it's not 100% necessary for
good welds, but should be a nice feature to have.
After this and some code clean-up problably we'll stop the development of the firmware
on this kind of hardware as the transformer is not the best option for a spot welder,
and we'll focus on other hardware options.


## License
This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
