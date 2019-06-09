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
#include <TimeAlarms.h>
#include <EEPROM.h>
#include <utils.h>
#include <timer.h>

int timerHour;
int timerMinute;

void RegularFeed(){
    Feed(regularFeed);
}

void initTimer(){
    EEPROM.begin(4);
    delay(10);
    if (EEPROM.read(0) == 1) {
        timerHour = EEPROM.read(1);
        timerMinute = EEPROM.read(2);
        Alarm.alarmRepeat(timerHour, timerMinute, 0, RegularFeed);
        Serial.println("Timer is set at " + String(timerHour, DEC) + ":" + String(timerMinute, DEC));
    }
    EEPROM.end();
}

void setTimer(int hour, int minute) {
    EEPROM.begin(4);
    delay(10);
    EEPROM.write(0, 1);
    EEPROM.write(1, hour);
    EEPROM.write(2, minute);
    EEPROM.commit();
    EEPROM.end();
    Serial.println("Timer data saved to EEPROM. Restarting.");
    ESP.restart();
}

void disableTimer(){
    EEPROM.begin(4);
    delay(10);
    EEPROM.write(0, 0);
    EEPROM.commit();
    EEPROM.end();
    Serial.println("Timer data saved to EEPROM. Restarting.");
    ESP.restart();
}