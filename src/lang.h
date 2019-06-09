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

#define en 0
#define pl 1

#ifdef USE_TELEGRAM
    #if language == pl
        #define tg_AUTH "Autoryzacja przebiegła pomyślnie."
        #define tg_WELCOME "Lista komend: \n/feed - Aby nakarmić \n/timer - Aby zmienić lub wyłączyć porę karmienia \n/stop - Aby wypisać się z listy użytkowników"
        #define tg_WRONG "Coś poszło nie tak."
        #define tg_UNSUB "Wypisano pomyślnie"
        #define tg_YUM "Mniam mniam"
        #define tg_FEED_SET "Czas karmienia został ustawiony. Restartuję urządzenie."
        #define tg_DISABLE "Wyłącz"
        #define tg_SELECT_HOUR "Wybierz godzinę karmienia"
        #define tg_FEED_DISABLE "Automatyczne karmienie zostało wyłączone. Restartuję urządzenie."
        #define tg_SELECT_MINUTE "Wybierz minutę karmienia"
        #define tg_OUTOFFOOD "Stan pokarmu w dozowniku jest niski. Uzupełnij braki świeżym pokarmem."
    #endif

    #if language == en
        #define tg_AUTH "Authorization has been granted successfully."
        #define tg_WELCOME "List of commands: \n/feed - Feed now \n/timer - Change/disable feeding time \n/stop - To unsubscribe"
        #define tg_WRONG "Something went wrong."
        #define tg_UNSUB "Unsubscribed successfully"
        #define tg_YUM "Yum yum"
        #define tg_FEED_SET "Feeding time has been set. Restarting the device."
        #define tg_DISABLE "Disable"
        #define tg_SELECT_HOUR "Choose hour"
        #define tg_FEED_DISABLE "Automatic feeding has been disabled. Restarting the device."
        #define tg_SELECT_MINUTE "Choose minute"
        #define tg_OUTOFFOOD "There is not enough food in the dispenser. Add more food to the dispenser."
    #endif

#endif