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

int toggleState_1 = 0; //Define integer to remember the toggle state for relay 1
int toggleState_2 = 0; //Define integer to remember the toggle state for relay 2
int toggleState_3 = 0; //Define integer to remember the toggle state for relay 3
int toggleState_4 = 0; //Define integer to remember the toggle state for relay 4
int toggleState_5 = 0; //Define integer to remember the toggle state for relay 5

// Update these with values suitable for your network.

//const char* ssid = "emg"; //WiFI Name
//const char* password = "haridy12"; //WiFi Password
const char* mqttServer = "04c198eaf0e344c0823a1870ebc46c19.s1.eu.hivemq.cloud";
const char* mqttUserName = "remoteera"; // MQTT username
const char* mqttPwd = "Remoteera10"; // MQTT password
const char* clientID = "remote"; // client id

int board_number=0;

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
  loadStates();
  delay(1000);
  Serial.begin(115200);
 //sub
  String temp="switch"+String(1+board_number*5)+board_id;
  temp.toCharArray(sub1,50);
  
  temp="switch"+String(2+board_number*5)+board_id;
  temp.toCharArray(sub2,50);
  
  temp="switch"+String(3+board_number*5)+board_id;
  temp.toCharArray(sub3,50);
  
  temp="switch"+String(4+board_number*5)+board_id;
  temp.toCharArray(sub4,50);
  
  temp="switch"+String(5+board_number*5)+board_id;
  temp.toCharArray(sub5,50);

//pub
  temp="switch"+String(1+board_number*5)+"_status"+board_id;
  temp.toCharArray(pub1,50);
  
  temp="switch"+String(2+board_number*5)+"_status"+board_id;
  temp.toCharArray(pub2,50);
  
  temp="switch"+String(3+board_number*5)+"_status"+board_id;
  temp.toCharArray(pub3,50);
  
  temp="switch"+String(4+board_number*5)+"_status"+board_id;
  temp.toCharArray(pub4,50);
  
  temp="switch"+String(5+board_number*5)+"_status"+board_id;
  temp.toCharArray(pub5,50);

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
  digitalWrite(RelayPin1, toggleState_1);
  digitalWrite(RelayPin2, toggleState_2);
  digitalWrite(RelayPin3, toggleState_3);
  digitalWrite(RelayPin4, toggleState_4);
  digitalWrite(RelayPin5, toggleState_5);
  
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
