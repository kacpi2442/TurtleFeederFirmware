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
#ifdef USE_TELEGRAM
#include <ArduinoJson.h>
void telegramInit();
void telegramHandle();
void handleNewMessages(int numNewMessages);
JsonObject& getSubscribedUsers();
bool addSubscribedUser(String chat_id, String from_name);
bool removeSubscribedUser(String chat_id);
bool isUserSubscribed(String chat_id);
void sendMessageToAllSubscribedUsers(String message);
#endif