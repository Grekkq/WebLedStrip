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

# How to add another remote:

- Make UI for new remote, if you feel like lazy you can just copy/paste the previous one. _(Order isn't important but i usually do it like that)_

- Make sure that you changed `blue` in html `onclick="ButtonPressed('blue')` to your own feature name.

- In main.cpp add reaction for new requests, you have to extend `void ConfigureWebpages(AsyncWebServer &server)`. If you want to sent just simple IR code use:
```cpp
server.on("/yourFeatureName", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending your IR code"); // This line is just so you can see result in console
        irsend.sendNEC(0xFF906F, 32); // Use your decoded code
        request->send(200); // Just so we can meet the requirements of http request
    });
```

- And that's all, now you can build and upload new code to your device. _(Don't forget to update "File System Image")_

# How to use decoder

1) First you have to create new project and replace automaticly genereted code with code from file "CodeDecoding". _(Or you can just temporaily comment whole main.cpp)_

2) After that you have to go to platformio.ini and comment out `lib_deps = ESP Async WebServer, ESPAsyncTCP, IRremoteESP8266` _(Just add `;` at the start of line)_ and uncomment `lib_deps = IRremoteESP8266`. After that hit Upload and Monitor.

3) When monitor start you can point your remote onto sensor. The decoded signal should start to appear after each button press. _(If you get inconsisten results try to press button just once and wait a moment, sometimes buffor that stores signal doesn't go back to zero in time)_