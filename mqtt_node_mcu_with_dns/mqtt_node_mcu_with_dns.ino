#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include <time.h>
#include <TZ.h>
#include <FS.h>
#include <LittleFS.h>
#include <CertStoreBearSSL.h>
/*
   This example serves a "hello world" on a WLAN and a SoftAP at the same time.
   The SoftAP allow you to configure WLAN parameters at run time. They are not setup in the sketch but saved on EEPROM.

   Connect your computer or cell phone to wifi network ESP_ap with password 12345678. A popup may appear and it allow you to go to WLAN config. If it does not then navigate to http://192.168.4.1/wifi and config it there.
   Then wait for the module to connect to your wifi and take note of the WLAN IP it got. Then you can disconnect from ESP_ap and return to your regular WLAN.

   Now the ESP8266 is in your network. You can reach it through http://192.168.x.x/ (the IP you took note of) or maybe at http://esp8266.local too.

   This is a captive portal because through the softAP it will redirect any http request to http://192.168.4.1/
*/

/* Set these to your desired softAP credentials. They are not configurable at runtime */


const char *softAP_ssid = "Remoteera";
const char *softAP_password = "remoteera";

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "esp8266";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[80] = "";
char password[65] = "";

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;



// Relays
#define RelayPin1 5  //D1
#define RelayPin2 4  //D2
#define RelayPin3 14  //D5
#define RelayPin4 12  //D6
#define RelayPin5 13  //D7

// Switches
#define SwitchPin1 10  //SD3
#define SwitchPin2 2   //D3 
#define SwitchPin3 0   //D7
#define SwitchPin4 3   //RX
#define SwitchPin5 1   //tX

//WiFi Status LED
#define wifiLed    16   //D0

int toggleState_1 = 1; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 1; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 1; //Define integer to remember the toggle state for relay 3
int toggleState_4 = 1; //Define integer to remember the toggle state for relay 4
int toggleState_5 = 1; //Define integer to remember the toggle state for relay 5

// Update these with values suitable for your network.

//const char* ssid = "emg"; //WiFI Name
//const char* password = "haridy12"; //WiFi Password
const char* mqttServer = "04c198eaf0e344c0823a1870ebc46c19.s1.eu.hivemq.cloud";
const char* mqttUserName = "remoteera"; // MQTT username
const char* mqttPwd = "Remoteera10"; // MQTT password
const char* clientID = "remo"; // client id


#define board_id "emg"

char sub1[50];  
char sub2[50];
char sub3[50] ;
char sub4[50];
char sub5[50];


char pub1[50];
char pub2[50];
char pub3[50];
char pub4[50];
char pub5[50];


char states[50];


BearSSL::CertStore certStore;
WiFiClientSecure espClient;
PubSubClient client;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (80)
char msg[MSG_BUFFER_SIZE];
int value = 0;


void setup() {
  delay(1000);
  Serial.begin(115200);
  
  
  strcat(strcat(sub1, "switch1"),board_id);
  strcat(strcat(sub2, "switch2"),board_id);
  strcat(strcat(sub3, "switch3"),board_id);
  strcat(strcat(sub4, "switch4"),board_id);
  strcat(strcat(sub5, "switch5"),board_id);

  strcat(strcat(pub1, "switch1_status"),board_id);
  strcat(strcat(pub2, "switch2_status"),board_id);
  strcat(strcat(pub3, "switch3_status"),board_id);
  strcat(strcat(pub4, "switch4_status"),board_id);
  strcat(strcat(pub5, "switch5_status"),board_id);
  strcat(strcat(states, "states"),board_id);

    
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);
  pinMode(SwitchPin5, INPUT_PULLUP);

  pinMode(wifiLed, OUTPUT);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);
  digitalWrite(RelayPin5, HIGH);
  
  //During Starting WiFi LED should TURN OFF
  digitalWrite(wifiLed, HIGH);
  //littleFs for flash memory
  LittleFS.begin();
    
  Serial.println();
  Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server.onNotFound(handleNotFound);
  server.begin(); // Web server start
  loadCredentials(); // Load WLAN credentials from network
  connect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID
  setup_mqtt();
  
}

void connectWifi() {
  Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  int connRes = WiFi.waitForConnectResult();
  Serial.print("connRes: ");
  Serial.println(connRes);
}

void loop() {
  if (connect) {
    Serial.println("Connect requested");
    connect = false;
    connectWifi();
    lastConnectTry = millis();
  }
  {
    unsigned int s = WiFi.status();
    if (s == 0 && millis() > (lastConnectTry + 60000)) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = true;
    }
    if (status != s) { // WLAN status change
      Serial.print("Status: ");
      Serial.println(s);
      status = s;
      if (s == WL_CONNECTED) {
        setDateTime();
      } else if (s == WL_NO_SSID_AVAIL) {
        WiFi.disconnect();
      }
    }
  }
  
      if (!client.connected()) {
      digitalWrite(wifiLed, HIGH);
      if(WiFi.status()==WL_CONNECTED){
       reconnect();
       }
    }else if(client.connected()){
      digitalWrite(wifiLed, LOW);
      manual_control();
    }
   client.loop();
  // Do work:
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
}
