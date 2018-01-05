#define DEFAULT_WIFI_SSID "il-mio-ssid"
#define DEFAULT_WIFI_PASS "password"

// (atrent) potresti prevedere un "fallback mode" che attiva un AP

void wifi_setup() {
  WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASS);
}

void wifi_waitOnline() {
  while(WiFi.status() != WL_CONNECTED) {
    delay(200);
  }
}

String wifi_info() {
  if (WiFi.status() == WL_CONNECTED) {
    return "Online, IP = " + WiFi.localIP().toString() + ", signal = " + WiFi.RSSI() + "dBm";
  } else {
    return "Offline";
  }
}
