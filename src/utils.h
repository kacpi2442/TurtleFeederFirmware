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
#include <WiFiUdp.h>
#include <config.h>

#define NTP_localPort 8888 // local port to listen for UDP packets
#define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message

void RGB_color(int red_light_value,
    int green_light_value,
    int blue_light_value);
time_t getNtpTime();
void sendNTPpacket(IPAddress& address);
void Feed(int times);
String getValue(String data, char separator, int index);
boolean outOfFood();
time_t cvt_date(char const* date, char const* time);
#ifdef useDST
int getDayOfWeek(int d, int m, int y);
boolean inSummerTime(time_t time);
#endif

extern WiFiUDP Udp;
