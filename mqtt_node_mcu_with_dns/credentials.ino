/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0 + sizeof(ssid), password);
  char ok[2 + 1];
  EEPROM.get(0 + sizeof(ssid) + sizeof(password), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(strlen(password) > 0 ? "********" : "<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0 + sizeof(ssid), password);
  char ok[2 + 1] = "OK";
  EEPROM.put(0 + sizeof(ssid) + sizeof(password), ok);
  EEPROM.commit();
  EEPROM.end();
}
void saveStates(){
  EEPROM.begin(512);


  EEPROM.put( sizeof(ssid)+sizeof(password)+3, toggleState_1);
  EEPROM.put(sizeof(ssid)+sizeof(password)+7, toggleState_2);
  EEPROM.put(sizeof(ssid)+sizeof(password)+11, toggleState_3);
  EEPROM.put(sizeof(ssid)+sizeof(password)+15, toggleState_4);
  EEPROM.put(sizeof(ssid)+sizeof(password)+19, toggleState_5);
  char ok[2 + 1] = "OK";
  EEPROM.put(0 + sizeof(ssid) + sizeof(password)+23, ok);
  EEPROM.commit();
  EEPROM.end();
  }
  
  void loadStates() {
  EEPROM.begin(512); 
  EEPROM.get( sizeof(ssid)+sizeof(password)+3, toggleState_1);
  EEPROM.get(sizeof(ssid)+sizeof(password)+7, toggleState_2);
  EEPROM.get(sizeof(ssid)+sizeof(password)+11, toggleState_3);
  EEPROM.get(sizeof(ssid)+sizeof(password)+15, toggleState_4);
  EEPROM.get(sizeof(ssid)+sizeof(password)+19, toggleState_5);
  char ok[2 + 1];
  EEPROM.get(0 + sizeof(ssid) + sizeof(password)+23, ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
}
