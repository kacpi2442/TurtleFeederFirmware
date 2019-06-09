/**
 * TurtleFeederFirmware
 * Copyright (C) 2019 kacpi2442 [https://github.com/kacpi2442/TurtleFeederFirmware]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <config.h>
#include <main.h>
#include <utils.h>
#ifdef USE_OTA
#include <OTA.h>
#endif
#ifdef USE_MQTT
#include <mqtt.h>
#endif
#ifdef USE_TELEGRAM
#include <telegram.h>
#endif
#include <timer.h>

const char* ssid = STASSID;
const char* password = STAPSK;

// Tracks the last time event fired
unsigned long previousMillis = 0;

// Used to track if LED should be on or off
boolean outOfFoodLED = false;

// Button variables
int buttonVal = 0; // value read from button
int buttonLast = 1; // buffered value of the button's previous state
unsigned long btnDnTime; // time the button was pressed down
unsigned long btnUpTime; // time the button was released

Servo feeder; // create servo object to control a servo

void setup() {
    Serial.begin(115200);
    Serial.println("Booting...");
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(led_RED, OUTPUT);
    pinMode(led_GREEN, OUTPUT);
    pinMode(led_BLUE, OUTPUT);
    pinMode(sensorPin, INPUT);
    feeder.attach(servoPin);
    feeder.write(homePosition);
    RGB_color(255, 0, 0);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && millis() < 15000) {
        yield();
    }
#ifdef USE_OTA
    SetupOTA();
#endif
    Udp.begin(NTP_localPort);
    setSyncProvider(getNtpTime);
#ifdef useDST
    if (inSummerTime(now())) {
        Serial.println("DST detected, adjusting time");
        setSyncProvider(getNtpTime); // Re-sync time
    }
#endif
#ifdef USE_MQTT
    mqttInit();
    mqttTryToConnect();
    Alarm.timerRepeat(60, mqttTryToConnect);
#endif
#ifdef USE_TELEGRAM
    telegramInit();
    Alarm.timerRepeat(5, telegramHandle);
#endif
    initTimer();
    Alarm.delay(500);
    feeder.detach();
    RGB_color(0, standbyIntensivity, 0);
}

void loop() {
    checkConnection();
    Alarm.delay(0);
#ifdef USE_MQTT
    client.loop();
#endif
#ifdef USE_OTA
    handleOTA();
#endif
    buttonHandle();
    ledHandle();
}

void checkConnection() {
    while (WiFi.status() != WL_CONNECTED) {
        RGB_color(255, 0, 0);
        Alarm.delay(0);
        buttonHandle();
    }
}

void buttonHandle() {
    // Read the state of the button
    buttonVal = digitalRead(buttonPin);

    // Test for button pressed and store the down time
    if (buttonVal == LOW && buttonLast == HIGH && (millis() - btnUpTime) > long(debounce))
        btnDnTime = millis();

    // Test for button release and store the up time
    if (buttonVal == HIGH && buttonLast == LOW && (millis() - btnDnTime) > long(debounce)) {
        // Short button press action
        Feed(1);
        btnUpTime = millis();
    }

    // Test for button held down for longer than the hold time
    if (buttonVal == LOW && (millis() - btnDnTime) > long(holdTime)) {
        // Long button press action
        ESP.restart();
    }
    buttonLast = buttonVal;
}

void ledHandle() {
    if (outOfFood()) {
        // If there is no food in dispenser
        // Grab snapshot of current time, this keeps all timing consistent
        unsigned long currentMillis = millis();
        // Compare to previous capture to see if enough time has passed
        if ((unsigned long)(currentMillis - previousMillis) >= ledBlinkTime) {
            outOfFoodLED = !(outOfFoodLED); // Invert state
            previousMillis = currentMillis; // Save the current time to compare "later"
            outOfFoodLED ? RGB_color(255, 64, 0) : RGB_color(0, 255, 0); // Set the LED according to state of variable
        }
    } else {
        RGB_color(0, standbyIntensivity, 0); // Default standby
    }
}