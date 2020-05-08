# LED Strip IR Remote with Web Interface

This project is created as a assignment for a university.

# Main aim of the project

* Web interface with remote like button layout to control the LED strip

* LED strip is controlled by IR signals genereted by microcontroller

* Small and portable format of the device

# Used parts:

- NodeMCU V2 based on ESP8266-12E

- IR transmitter SFH4550

- 330Ω rezistor

- Any LED Strip that can be controlled by IR remote

- USB cable to program and supply power to microcontroller

# Used libraries and other sources:

* [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266 "lib")

* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer "lib")

* [AsyncTCP](https://github.com/me-no-dev/AsyncTCP "lib")

* [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP "lib")

* [IR CODES](http://woodsgood.ca/projects/2015/02/13/rgb-led-strip-controllers-ir-codes/ "other")

* [Simple LED calculator](http://led.linear1.org/1led.wiz "other")

# How to compile my own version:

1) You have to download source code from Github, and get PlatformIO. _(Personally I recommend just getting platformio plugin for Visual Studio Code but it is up to you)_

2) Now you can chang code however you like and when ready you will build your solution.

3) Hopefully there were no errors. If so connect your NodeMCU with USB cable to the computer and hit upload. _(If you made any changes int "data" folder you will also have to update "File System Image")_

 4) And that's all, if everything works as expected you are good to go. Now you can disconnect the device and set it up on the desired spot.