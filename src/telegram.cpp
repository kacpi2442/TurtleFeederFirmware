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
#include <ESP8266WiFi.h>
#include <FS.h>
#include <TimeLib.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <telegram.h>
#include <timer.h>
#include <utils.h>
#include <lang.h>

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOTtoken, secured_client);


int bulk_messages_mtbs = 1500; // mean time between send messages, 1.5 seconds
int messages_limit_per_second = 25; // Telegram API have limit for bulk messages ~30 messages per second

String subscribed_users_filename = "subscribed_users.json";

int numNewMessages;

DynamicJsonBuffer jsonBuffer;

void handleNewMessages(int numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
        String message_type = bot.messages[i].type;
        String command = getValue(text, ' ', 0);

        if (text == String("/start ") + telegramSecret) {
            if (addSubscribedUser(chat_id, from_name)) {
                bot.sendMessage(chat_id, tg_AUTH, "");
                String keyboardJson = "[[\"/feed\", \"/timer\"]]";
                bot.sendMessageWithReplyKeyboard(chat_id, tg_WELCOME, "", keyboardJson, true);
            } else {
                bot.sendMessage(chat_id, tg_WRONG, "");
            }
        }
        if (isUserSubscribed(chat_id)) {
            if (command == "/stop") {
                if (removeSubscribedUser(chat_id)) {
                    bot.sendMessage(chat_id, tg_UNSUB, "");
                } else {
                    bot.sendMessage(chat_id, tg_WRONG, "");
                }
            }
            if (command == "/feed") {
                bot.sendMessage(chat_id, tg_YUM, "");
                Feed(regularFeed);
            }
            if (command == "/time") {
                bot.sendMessage(chat_id, "Time now: " + String(hour()) + ":" + String(minute()), "");
            }
            if (command == "/timentp") {
                bot.sendMessage(chat_id, "NTP Time now: " + String(getNtpTime()), "");
            }
            if (command == "/timer") {
                String arg1 = getValue(text, ' ', 1);
                String arg2 = getValue(text, ' ', 2);
                if (arg2 != "") {
                    bot.sendMessage(chat_id, tg_FEED_SET, "");
                    numNewMessages
                        = bot.getUpdates(bot.last_message_received + 1); // Set this message as already handled to prevent loop
                    setTimer(arg1.toInt(), arg2.toInt());
                } else {
                    String keyboardJson = "[[{ \"text\" : \"0\", \"callback_data\" : \"hour=0\" },";
                    keyboardJson += "{ \"text\" : \"1\", \"callback_data\" : \"hour=1\" },";
                    keyboardJson += "{ \"text\" : \"2\", \"callback_data\" : \"hour=2\" },";
                    keyboardJson += "{ \"text\" : \"3\", \"callback_data\" : \"hour=3\" }],";
                    keyboardJson += "[{ \"text\" : \"4\", \"callback_data\" : \"hour=4\" },";
                    keyboardJson += "{ \"text\" : \"5\", \"callback_data\" : \"hour=5\" },";
                    keyboardJson += "{ \"text\" : \"6\", \"callback_data\" : \"hour=6\" },";
                    keyboardJson += "{ \"text\" : \"7\", \"callback_data\" : \"hour=7\" }],";
                    keyboardJson += "[{ \"text\" : \"8\", \"callback_data\" : \"hour=8\" },";
                    keyboardJson += "{ \"text\" : \"9\", \"callback_data\" : \"hour=9\" },";
                    keyboardJson += "{ \"text\" : \"10\", \"callback_data\" : \"hour=10\" },";
                    keyboardJson += "{ \"text\" : \"11\", \"callback_data\" : \"hour=11\" }],";
                    keyboardJson += "[{ \"text\" : \"12\", \"callback_data\" : \"hour=12\" },";
                    keyboardJson += "{ \"text\" : \"13\", \"callback_data\" : \"hour=13\" },";
                    keyboardJson += "{ \"text\" : \"14\", \"callback_data\" : \"hour=14\" },";
                    keyboardJson += "{ \"text\" : \"15\", \"callback_data\" : \"hour=15\" }],";
                    keyboardJson += "[{ \"text\" : \"16\", \"callback_data\" : \"hour=16\" },";
                    keyboardJson += "{ \"text\" : \"17\", \"callback_data\" : \"hour=17\" },";
                    keyboardJson += "{ \"text\" : \"18\", \"callback_data\" : \"hour=18\" },";
                    keyboardJson += "{ \"text\" : \"19\", \"callback_data\" : \"hour=19\" }],";
                    keyboardJson += "[{ \"text\" : \"20\", \"callback_data\" : \"hour=20\" },";
                    keyboardJson += "{ \"text\" : \"21\", \"callback_data\" : \"hour=21\" },";
                    keyboardJson += "{ \"text\" : \"22\", \"callback_data\" : \"hour=22\" },";
                    keyboardJson += "{ \"text\" : \"23\", \"callback_data\" : \"hour=23\" }],";
                    keyboardJson += "[{ \"text\" : \"" + String(tg_DISABLE) + "\", \"callback_data\" : \"t_disable\" }]]";
                    bot.sendMessageWithInlineKeyboard(chat_id, tg_SELECT_HOUR, "", keyboardJson);
               }
            }
            if (message_type == "callback_query") { // Received when user taps a button on inline keyboard
                // In our case, callback_query is only received for PWM values. In other cases you may
                // want to append an identifier to the values sent in 'callback_data' (e.g. 'duty=10')
                String type = getValue(text, '=', 0);
                if (type == "t_disable") {
                    numNewMessages
                        = bot.getUpdates(bot.last_message_received + 1); // Set this message as already handled to prevent loop
                    bot.sendMessage(chat_id, tg_FEED_DISABLE, "");
                    disableTimer();
                }
                if (type == "hour") {
                    String h = getValue(text, '=', 1);
                    String keyboardJson = "[[{ \"text\" : \"0\", \"callback_data\" : \"hm=" + h + ",0\" },";
                    keyboardJson += "{ \"text\" : \"15\", \"callback_data\" : \"hm=" + h + ",15\" }],";
                    keyboardJson += "[{ \"text\" : \"30\", \"callback_data\" : \"hm=" + h + ",30\" },";
                    keyboardJson += "{ \"text\" : \"45\", \"callback_data\" : \"hm=" + h + ",45\" }]]";
                    bot.sendMessageWithInlineKeyboard(chat_id, tg_SELECT_MINUTE, "", keyboardJson);
                }
                if (type == "hm"){
                    String hm = getValue(text, '=', 1);
                    numNewMessages
                        = bot.getUpdates(bot.last_message_received + 1); // Set this message as already handled to prevent loop
                    bot.sendMessage(chat_id, tg_FEED_SET, "");
                    setTimer(getValue(hm, ',', 0).toInt(), getValue(hm, ',', 1).toInt());
                }
            }
        }
    }
}

JsonObject& getSubscribedUsers() {
    File subscribedUsersFile = SPIFFS.open("/" + subscribed_users_filename, "r");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file");

        // Create empty file (w+ not working as expect)
        File f = SPIFFS.open("/" + subscribed_users_filename, "w");
        f.close();

        JsonObject& users = jsonBuffer.createObject();

        return users;
    } else {
        size_t size = subscribedUsersFile.size();

        if (size > 1024) {
            Serial.println("Subscribed users file is too large");
            // return users;
        }

        String file_content = subscribedUsersFile.readString();

        JsonObject& users = jsonBuffer.parseObject(file_content);

        if (!users.success()) {
            Serial.println("Failed to parse subscribed users file");
            return users;
        }

        subscribedUsersFile.close();

        return users;
    }
}

bool addSubscribedUser(String chat_id, String from_name) {
    JsonObject& users = getSubscribedUsers();

    File subscribedUsersFile = SPIFFS.open("/" + subscribed_users_filename, "w+");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file for writing");
        // return false;
    }

    users.set(chat_id, from_name);
    users.printTo(subscribedUsersFile);

    subscribedUsersFile.close();

    return true;
}

bool removeSubscribedUser(String chat_id) {
    JsonObject& users = getSubscribedUsers();

    File subscribedUsersFile = SPIFFS.open("/" + subscribed_users_filename, "w");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file for writing");
        return false;
    }

    users.remove(chat_id);
    users.printTo(subscribedUsersFile);

    subscribedUsersFile.close();

    return true;
}

bool isUserSubscribed(String chat_id) {
    JsonObject& users = getSubscribedUsers();
    return users.containsKey(chat_id);
}

void sendMessageToAllSubscribedUsers(String message) {
    int users_processed = 0;

    JsonObject& users = getSubscribedUsers();

    for (JsonObject::iterator it = users.begin(); it != users.end(); ++it) {
        users_processed++;

        if (users_processed < messages_limit_per_second) {
            const char* chat_id = it->key;
            bot.sendMessage(chat_id, message, "");
        } else {
            delay(bulk_messages_mtbs);
            users_processed = 0;
        }
    }
}

void telegramInit() {
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }
    //bot._debug = true;
    bot.maxMessageLength = 1800;
}

void telegramHandle() {
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
}

#endif