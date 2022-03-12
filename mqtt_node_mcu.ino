#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>

#include <time.h>
#include <TZ.h>
#include <FS.h>
#include <LittleFS.h>
#include <CertStoreBearSSL.h>

int re_wifi=0,re_mqtt=0;

const char *softAP_ssid = "Remoteera";
const char *softAP_password = "remoteera";

char ssid[33] = "";
char password[65] = "";


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);
DNSServer dnsServer;
ESP8266WebServer server(80);
const char *myHostname = "esp8266";


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
bool m_set_up=true,connect=true;
void reconnect() {
  if(m_set_up){
  m_set_up=false;
  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    Serial.printf("Number of CA certs read: %d\n", numCerts);
    if (numCerts == 0) {
        Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
        return; // Can't connect to anything w/o certs!
    }
    BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
    // Integrate the cert store with this connection
    bear->setCertStore(&certStore);

    client = *(new PubSubClient(*bear));
    
   client.setServer(mqttServer, 8883);
   client.setCallback(callback);}
  {
 if (client.connect(clientID, mqttUserName, mqttPwd)) {
 Serial.println("MQTT connected");
      // ... and resubscribe
      client.subscribe(sub1);
      client.subscribe(sub2);
      client.subscribe(sub3);
      client.subscribe(sub4);
      client.subscribe(sub5);
      client.subscribe(states);
    } 
    else {
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (strstr(topic, sub1))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_1 = 0;
      client.publish(pub1, "0");
    } else {
      digitalWrite(RelayPin1, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_1 = 1;
      client.publish(pub1, "1");
    }    
  }

  else if ( strstr(topic, sub2))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin2, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_2 = 0;
      client.publish(pub2, "0");
    } else {
      digitalWrite(RelayPin2, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_2 = 1;
      client.publish(pub2, "1");
    }
  }
  else if ( strstr(topic, sub3))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin3, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_3 = 0;
      client.publish(pub3, "0");
    } else {
      digitalWrite(RelayPin3, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_3 = 1;
      client.publish(pub3, "1");
    }
  }
  else if ( strstr(topic, sub4))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin4, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_4 = 0;
      client.publish(pub4, "0");
    } else {
      digitalWrite(RelayPin4, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_4 = 1;
      client.publish(pub4, "1");
    }
  }
  else if ( strstr(topic, sub5))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin5, LOW);   // Turn the LED on (Note that LOW is the voltage level
      toggleState_5 = 0;
      client.publish(pub5, "0");
    } else {
      digitalWrite(RelayPin5, HIGH);  // Turn the LED off by making the voltage HIGH
      toggleState_5 = 1;
      client.publish(pub5, "1");
    }
  }
  else if ( strstr(topic, states)){
     client.publish(pub5, toggleState_1?"1":"0");
     client.publish(pub4, toggleState_4?"1":"0");
     client.publish(pub3, toggleState_3?"1":"0");
     client.publish(pub2, toggleState_2?"1":"0");
     client.publish(pub1, toggleState_1?"1":"0");
  }
  else
  {
    Serial.println("unsubscribed topic");
  }
  delay(200);
}

void manual_control(){
 Serial.println("manual control");
    //Manual Switch Control
    if (digitalRead(SwitchPin1) == LOW){
      delay(200);
      if(toggleState_1 == 1){
        digitalWrite(RelayPin1, LOW); // turn on relay 1
        toggleState_1 = 0;
        client.publish(pub1, "0");
        Serial.println("Device1 ON");
        }
      else{
        digitalWrite(RelayPin1, HIGH); // turn off relay 1
        toggleState_1 = 1;
        client.publish(pub1, "1");
        Serial.println("Device1 OFF");
        }
     }
    else if (digitalRead(SwitchPin2) == LOW){
      delay(200);
      if(toggleState_2 == 1){
        digitalWrite(RelayPin2, LOW); // turn on relay 2
        toggleState_2 = 0;
        client.publish(pub2, "0");
        Serial.println("Device2 ON");
        }
      else{
        digitalWrite(RelayPin2, HIGH); // turn off relay 2
        toggleState_2 = 1;
        client.publish(pub2, "1");
        Serial.println("Device2 OFF");
        }
    }
    else if (digitalRead(SwitchPin3) == LOW){
      delay(200);
      if(toggleState_3 == 1){
        digitalWrite(RelayPin3, LOW); // turn on relay 3
        toggleState_3 = 0;
        client.publish(pub3, "0");
        Serial.println("Device3 ON");
        }
      else{
        digitalWrite(RelayPin3, HIGH); // turn off relay 3
        toggleState_3 = 1;
        client.publish(pub3, "1");
        Serial.println("Device3 OFF");
        }
    }
    else if (digitalRead(SwitchPin4) == LOW){
      delay(200);
      if(toggleState_4 == 1){
        digitalWrite(RelayPin4, LOW); // turn on relay 4
        toggleState_4 = 0;
        client.publish(pub4, "0");
        Serial.println("Device4 ON");
        }
      else{
        digitalWrite(RelayPin4, HIGH); // turn off relay 4
        toggleState_4 = 1;
        client.publish(pub4, "1");
        Serial.println("Device4 OFF");
        }
    }
    else if (digitalRead(SwitchPin5) == LOW){
      delay(200);
      if(toggleState_5 == 1){
        digitalWrite(RelayPin5, LOW); // turn on relay 4
        toggleState_5 = 0;
        client.publish(pub5, "0");
        Serial.println("Device5 ON");
        }
      else{
        digitalWrite(RelayPin5, HIGH); // turn off relay 4
        toggleState_5 = 1;
        client.publish(pub5, "1");
        Serial.println("Device5 OFF");
        }
    }
    delay(100);
}
void setDateTime() {
    // You can use your own timezone, but the exact time is not used at all.
    // Only the date is needed for validating the certificates.
    configTime(TZ_Europe_Berlin, "pool.ntp.org", "time.nist.gov");

    Serial.print("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(100);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println();

    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.printf("%s %s", tzname[0], asctime(&timeinfo));
}
unsigned int status = WL_IDLE_STATUS;
void setup() {
  Serial.begin(9600);
  
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


  start_dns_server();
    
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
    
 
  LittleFS.begin();
   
  setDateTime();
}

void loop() {
  if(connect){
      WiFi.disconnect();
      WiFi.begin(ssid, password); 
      re_wifi=millis();
      connect=false;
      }
  unsigned int s = WiFi.status();
   
  if(s==0&&millis()>(re_wifi+60000)){
    connect=true;
   }
   if(status!=s){
    status=s;
    if(s==WL_CONNECTED){
        
      }
    else  {
        WiFi.disconnect();
      }
    }
    
     if (s == WL_CONNECTED&&!client.connected()) {
      digitalWrite(wifiLed, HIGH);
      reconnect();
      re_mqtt=millis(); 
    }else if(client.connected()){
      digitalWrite(wifiLed, LOW);
      manual_control();
    }
   client.loop();
     dnsServer.processNextRequest();
  server.handleClient();
 
}
