#include <networking.h>

void readWifiCredential(String &wf_ssid, String &wf_pass, Preferences &pref) {
  pref.begin("wifi_cred", false);
  wf_ssid = pref.getString("WF_SSID", WIFI_SSID);
  wf_pass = pref.getString("WF_PASS", WIFI_PASSWORD);
  pref.end();
}