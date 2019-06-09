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
#ifdef USE_MQTT
#include <main.h>
#include <mqtt.h>
#include <utils.h>

const char* mqtttopic = mqttTopic;
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    String s = String((char*)payload);
    Serial.println("Received mqtt message: " + s);
    int f = s.toInt();
    RGB_color(128, 64, 0);
    Feed(f);
    RGB_color(0, standbyIntensivity, 0);
}

void mqttInit() {
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

void mqttTryToConnect() {
    if (!client.connected()) {
        Serial.println("Trying to connect to MQTT...");
        if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
            Serial.println("connected");
            client.subscribe(mqttTopic);
        } else {
            Serial.print("failed with state ");
            Serial.println(client.state());
        }
    }
}
#endif