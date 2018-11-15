
/*
This sketch uses an HC-SR501 PIR and a Wemos D1 Mini.
When the PIR is triggered it sends a message to Ubidots.
Connection is WiFi and uses WiFiManager

Author:  Robin Harris
Date:   13th November 2018
*/

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <UbidotsESPMQTT.h>
#include <Ticker.h>
#include <NTPtimeESP.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
strDateTime dateTime; // object to hold date and time from NTP
Ticker tick; // object to be main system time keeper

NTPtime NTPuk("uk.pool.ntp.org");          // Choose server pool as required
char *ssid      = "Kercem2";               // Set you WiFi SSID
char *password  = "E0E3106433F4";          // Set you WiFi password


//set up for Ubidots
char *TOKEN = "A1E-AJ77mQYlznENTs3ZUuKFF3q4wddaqo";
// const char *password = "";//ignored by Ubidots
Ubidots ubidotsClient(TOKEN);

// #define WIFINAME "workshop" 
// #define WIFIPASS "workshop" 

#define pirPin D2 //pin used to monitor PIR GPIO5 == D1
#define ledPin D3 //used to show when the PIR is active GPIO14 == D5

//global variables
const char* mqtt_server = "192.168.0.36";
bool pirActive, pirPinStatus;
byte hour,minute,second = 0;
int count = 0; // number of triggers this period

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect(){
  // Loop until we're reconnected
  while (!mqttClient.connected()){
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("peopleCounter")){
      Serial.println("connected");
    }
    else{
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} // end reconnect

void sendBucket(){ // sends the count for the last 30 minutes and resets
  if (!ubidotsClient.connected()){
      ubidotsClient.reconnect();
  }
  ubidotsClient.add("count", count);
  ubidotsClient.ubidotsPublish("my-new-device");
  count = 0;
} // end sendBucket

void timer (){
  second++;
  if (second % 60 == 0){ // reset when reached 60 and increment minutes
    minute++;
    second = 0;
    if (minute % 60 == 0){ // reset when reached 60 and increment hours
      hour++;
      minute = 0;
    }
  }
} // end timer

void setup(){
    Serial.begin(115200);
    pinMode(pirPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); // turn off the led
    // Connect to wifi
	// WiFiManager wifiManager;
	// wifiManager.autoConnect("PeopleCounter");
    WiFi.mode(WIFI_STA);
    WiFi.begin (ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  NTPuk.setSendInterval(60);
  tick.attach(1, timer); // calls timer every second
  mqttClient.setServer(mqtt_server, 1883);
  ubidotsClient.setDebug(false); // Pass a true or false bool value to activate debug messages
  ubidotsClient.wifiConnection(ssid, password);
  ubidotsClient.begin(callback);
} 

void loop (){
    pirPinStatus = digitalRead(pirPin);
    if (pirPinStatus  && !pirActive){ // if we are seeing the pirPin going high at the beginning of a trigger
        pirActive = true;
        reconnect();
        mqttClient.publish("pir", "Triggered");
        count++;
    }
    if (!pirPinStatus && pirActive){ // if we see the pirPin go low and the status is ACTIVE
        pirActive = false;
    }
    digitalWrite(ledPin, pirActive); // set the led high when PIR active
    // ubidotsClient.loop();
    
    dateTime = NTPuk.getNTPtime(0.0, 1);
    if (dateTime.valid){
      NTPuk.printDateTime(dateTime);
      hour = dateTime.hour;
      minute = dateTime.minute;
      second = dateTime.second;
    }
    if (minute == 0 || minute == 30){
      sendBucket();
    }
} //end loop
