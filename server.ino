
void start_dns_server(){
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid/*, softAP_password*/);
  delay(500); // Without delay I've seen the IP address blank
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

   server.on("/", handleRoot);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.onNotFound(handleNotFound);
  server.begin(); // Web server start
  Serial.println("HTTP server started");
  loadCredentials(); // Load WLAN credentials from network
  connect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID


/*
  webServer.onNotFound([]() {
      String ur_i=webServer.uri();
      if(ur_i[1]=='*'){
        Serial.println(ur_i);
        }
        else if(ur_i[1]=='#'){
            
          }
      webServer.send(200, "text/plain", ur_i);
    });
    webServer.begin();*/
}
