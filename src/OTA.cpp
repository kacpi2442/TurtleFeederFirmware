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

#include <config.h>
#ifdef USE_OTA
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <OTA.h>
#include <main.h>
#include <utils.h>

void SetupOTA() {
    ArduinoOTA.setHostname(OTAhostname);
    ArduinoOTA.setPassword(OTAPassword);
    ArduinoOTA.onStart([]() {
        Serial.println("OTA Update");
        Serial.println("Progress: 0%");
        RGB_color(0, 0, 255);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
        RGB_color(0, 0, 255);
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        //feeder.write((progress / (total / 100)) * 1.8);
        if (((progress / (total / 100)) & 1) == 0) {
            RGB_color(0, 0, 0);
        } else {
            RGB_color(0, 0, 255);
        }
    });
    ArduinoOTA.onError([](ota_error_t error) {
        RGB_color(255, 0, 255);
        Serial.println("Update failed");
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
        delay(10000);
        ESP.restart();
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}
void handleOTA() {
    ArduinoOTA.handle();
}
#endif