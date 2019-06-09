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

#include <ESP8266WiFi.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <config.h>
#include <main.h>
#include <utils.h>
#ifdef USE_TELEGRAM
#include <telegram.h>
#include <lang.h>
#endif

IPAddress timeServerIP;
byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming & outgoing packets
WiFiUDP Udp;

time_t lastKnownTime = cvt_date(__DATE__, __TIME__); // Compile time
unsigned long lastKnownTimeAT = 0;

void RGB_color(int red_light_value,
    int green_light_value,
    int blue_light_value) {
    analogWrite(led_RED, red_light_value);
    analogWrite(led_GREEN, green_light_value);
    analogWrite(led_BLUE, blue_light_value);
}

void Feed(int times) {
    RGB_color(128, 64, 0);
    feeder.attach(servoPin);
    for (int i = 0; i < times; i++) {
        feeder.write(180);
        delay(600);
        feeder.write(homePosition);
        delay(600);
    }
    delay(600); // Extra delay before detaching servo
    feeder.detach();
#ifdef USE_TELEGRAM
    if (outOfFood()) {
        sendMessageToAllSubscribedUsers(tg_OUTOFFOOD);
    }
#endif
    RGB_color(0, standbyIntensivity, 0);
}

boolean outOfFood() {
    return analogRead(sensorPin) > sensorTrigger;
}

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#ifdef useDST
// Gives day of week
int getDayOfWeek(int d, int m, int y) {
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    y -= m < 3;
    // 0 Sunday, 1 Monday, etc...
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d + 6) % 7;
}

// Detects if we are in summer time
boolean inSummerTime(time_t time) {
    int hours = hour(time);
    int days = day(time);
    int months = month(time);
    int years = year(time);
    if (days < 0 || months < 0 || years < 0)
        return false;
    if ((months >= 3) && (months <= 10)) { // March to October inclusive
        if ((months > 3) && (months < 10)) { // Definitely
            return true;
        }
        if (months == 3) { // Have we passed the last Sunday of March, 1am UT ?
            if (days >= 25) { // It's getting interesting
                int dw = getDayOfWeek(days, months, years);
                // When is the next sunday ?
                int dts = 6 - dw; // Number of days before the next sunday
                if (dts == 0)
                    dts = 7; // We are really speaking of the next sunday, not the current one
                if ((days + dts) > 31) { // The next sunday is next month !
                    if (dw != 6 || hours > 0) // We finally check that we are not on the day of the change before the time of the change
                        return true;
                }
            }
        }
        if (months == 10) { // Have we passed the last Sunday of October 1am UT ?
            if (days >= 25) { // It's getting interesting
                int dw = getDayOfWeek(days, months, years);
                // When is the next sunday ?
                int dts = 6 - dw; // Number of days before the next sunday
                if (dts == 0)
                    dts = 7; // We are really speaking of the next sunday, not the current one
                if ((days + dts) > 31) { // The next sunday is next month !
                    if (dw != 6 || hours > 0) // We finally check that we are not on the day of the change before the time of the change
                        return false; // We have passed the change
                    else
                        return true;
                } else
                    return true;
            } else
                return true;
        }
    }
    return false;
}
#endif

time_t cvt_date(char const* date, char const* time) {
    char s_month[5];
    int year;
    tmElements_t t;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(date, "%s %hhd %d", s_month, &t.Day, &year);
    sscanf(time, "%2hhd %*c %2hhd %*c %2hhd", &t.Hour, &t.Minute, &t.Second);
    // Find where is s_month in month_names. Deduce month value.
    t.Month = (strstr(month_names, s_month) - month_names) / 3 + 1;
    // year can be given as '2010' or '10'. It is converted to years since 1970
    if (year > 99)
        t.Year = year - 1970;
    else
        t.Year = year + 30;
    return makeTime(t);
}

time_t getNtpTime() {
    while (Udp.parsePacket() > 0)
        ; // discard any previously received packets
    WiFi.hostByName(ntpServerName, timeServerIP);
    Serial.println("Transmit NTP Request");
    sendNTPpacket(timeServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            Serial.println("Receive NTP Response");
            Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
#ifdef useDST
            if (inSummerTime(lastKnownTime + (millis() - lastKnownTimeAT)/1000)) { // We can't use "now()" instead here becouse it will make a loop
                lastKnownTime = secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR + DSTshift * SECS_PER_MIN; // Add DSTshift (by default 60 minutes) to gathered time if we have now day saving time
                lastKnownTimeAT = millis();
                return lastKnownTime;
            }
#endif
            lastKnownTime = secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
            lastKnownTimeAT = millis();
            return lastKnownTime;
        }
    }
    Serial.println("No NTP Response :(");
    return lastKnownTime + (millis() - lastKnownTimeAT)/1000; // Return last known time (we can't use "now()" here becouse it will make a loop)
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress & address) {
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0; // Stratum, or type of clock
    packetBuffer[2] = 6; // Polling Interval
    packetBuffer[3] = 0xEC; // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); // NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}