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

// Wifi
#define STASSID "MyWifi"    // SSID of your wifi network
#define STAPSK "MyPassword"    // Password of your wifi network

// OTA
//#define USE_OTA    // Uncomment to enable upgrading firmware via espota
#ifdef USE_OTA
    #define OTAhostname "TurtleFeederInator"    // mDNS hostname
    #define OTAPassword "Password123"    // Password for upgrading firmware via espota
#endif

// Telegram
//#define USE_TELEGRAM    // Uncomment to enable Telegram Bot feature
#ifdef USE_TELEGRAM
    #define BOTtoken "123456789:xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"    // your Bot Token (Get from Botfather)
    #define telegramSecret "MySecret"
    #define language 0    // [en - 0, pl - 1]
#endif

// MQTT
//#define USE_MQTT    // Uncomment to enable MQTT feature
#ifdef USE_MQTT
    #define mqttServer "192.168.1.xxx"    // Address/IP of your MQTT broker
    #define mqttPort 1883
    #define mqttUser "user"
    #define mqttPassword "Password123"
    #define mqttTopic "turtlefeeder/feed"
#endif

// Time
#define ntpServerName "pl.pool.ntp.org"    // Time server pool address
#define timeZone 1    // Your timezone (CET by default)
#define useDST    // Comment this line if your country doesn't use daylight saving time
#ifdef useDST
    #define DSTshift 60    // Time shift of DST in minutes
#endif

// Misc
#define regularFeed 1    // Determines how big a portion should be
#define sensorTrigger 400    // (0-1023) lower is more sensitive
#define homePosition 8    // (0-180) Default servo position in degrees
#define standbyIntensivity 8    // (0-254) Intensivity of standby green led

// Pins
#define led_RED D7
#define led_GREEN D6
#define led_BLUE D5
#define buttonPin D3    // Button should connect to ground when pushed
#define servoPin D0    // SG-90 Servo yellow cable
#define sensorPin A0    // IR Photodiode connected between 5V and A0

// Times
#define debounce 15    // ms debounce period to prevent flickering when pressing or releasing the button
#define holdTime 3000    // ms hold period: how long to wait for press+hold event
#define ledBlinkTime 500    // For signaling out of food
