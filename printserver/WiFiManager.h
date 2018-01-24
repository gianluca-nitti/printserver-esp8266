/*
    This file is part of printserver-esp8266.

    printserver-esp8266 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    printserver-esp8266 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with printserver-esp8266.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <Arduino.h>
#include <functional>

class WiFiManager {
  private:
    static bool apEnabled;
  public:
    static void wifi_setup();
    static String info();
    static String getIP();
    static char* getEncryptionTypeName(int i);
    static void getAvailableNetworks(std::function<void(String, int, int)> forEachNet);
    static void connectTo(String ssid, String password);
};
