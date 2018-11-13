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

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//set up for Ubidots
char *TOKEN = "A1E-AJ77mQYlznENTs3ZUuKFF3q4wddaqo";
const char *password = "";//ignored by Ubidots
Ubidots ubidotsClient(TOKEN);

#define WIFINAME "workshop" 
#define WIFIPASS "workshop" 

#define pirPin 5 //pin used to monitor PIR GPIO5 == D1
#define ledPin 14 //used to show when the PIR is active GPIO14 == D5

//global variables
const char* mqtt_server = "192.168.0.36";
bool pirActive, pirPinStatus;

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

void setup(){
    Serial.begin(115200);
    pinMode(pirPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); // turn off the led
    // Connect to wifi
	// WiFiManager wifiManager;
	// wifiManager.autoConnect("PeopleCounter");
    Serial.println("Starting.....");
    mqttClient.setServer(mqtt_server, 1883);
    ubidotsClient.setDebug(false); // Pass a true or false bool value to activate debug messages
    ubidotsClient.wifiConnection(WIFINAME, WIFIPASS);
    ubidotsClient.begin(callback);
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    // ubidotsClient.ubidotsSetBroker("industrial.api.ubidots.com");
} 

void loop (){
    pirPinStatus = digitalRead(pirPin);
    if (pirPinStatus  && !pirActive){ // if we are seeing the pirPin going high at the beginning of a trigger
        pirActive = true;
        reconnect();
        mqttClient.publish("pir", "Triggered");
        if (!ubidotsClient.connected()){
            ubidotsClient.reconnect();
        }
        ubidotsClient.add("voltage", 5.3);
        ubidotsClient.ubidotsPublish("my-new-device");
    }
    if (!pirPinStatus && pirActive){ // if we see the pirPin go low and the status is ACTIVE
        pirActive = false;
    }
    digitalWrite(ledPin, pirActive); // set the led high when PIR active
    ubidotsClient.loop();
} //end loop
