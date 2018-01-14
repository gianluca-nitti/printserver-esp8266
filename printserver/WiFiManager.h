#include <Arduino.h>
#include <functional>

class WiFiManager {
  private:
    static bool apEnabled;
  public:
    static void wifi_setup();
    static String info();
    static char* getEncryptionTypeName(int i);
    static void getAvailableNetworks(std::function<void(String, int, int)> forEachNet);
    static void connectTo(String ssid, String password);
};
