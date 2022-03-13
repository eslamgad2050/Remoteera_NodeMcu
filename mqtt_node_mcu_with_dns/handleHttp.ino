/** Handle root or redirect to captive portal */
void handleRoot() {
  }

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

/** Wifi config page handler */
void handleWifi() {}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {}

void handleNotFound() {
  String ur=server.uri();
  if(ur.indexOf("ssid")>-1){
      String temp=ur.substring(ur.indexOf("ssid")+4,ur.indexOf("pass"));
      temp.toCharArray(ssid,sizeof(ssid));
      temp=ur.substring(ur.indexOf("pass")+4,ur.indexOf("?"));
      temp.toCharArray(password,sizeof(password));
      server.send(200, "text/plain", ssid);  
  
      if(strlen(ssid)>0){
        saveCredentials();
        connect=true;
        }
      }

}
