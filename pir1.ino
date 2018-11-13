/*
Demonstration of a 'people counter' using a Wemos D1 Mini and an HC-SR505 PIR
The Wemos is turned off until the PIR raises the 'detected' line.  The Wemos
holds itself on until the PIR has reset and the WiFi has been turned off.
It is necessary to turn off the WiFi as the signal activates the PIR so locking
the device permanently on.

Robin Harris
14th OCtober 2018
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <UbidotsESPMQTT.h>

#define WIFINAME "workshop" 
#define WIFIPASS "workshop" 

//set up for Ubidots
char *TOKEN = "BBFF-QZ1Z64rfnqpzCvZ0CXNsElGHDokCv8";
const char *password = "";//ignored by Ubidots

int holdPin = 4; // defines GPIO4 D2 as the hold pin (will hold CH_PD high untill we power down).
int pirPin = 5;  // defines GPIO5 D1 as the PIR read pin (reads the state of the PIR output).
int a0Value = 0;//holds A0 reading to convert to voltage
float voltage;
boolean sent = false;

Ubidots client(TOKEN);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
    pinMode(holdPin, OUTPUT); // sets GPIO4 to output
    digitalWrite(holdPin, HIGH);
    pinMode(pirPin, INPUT); // sets GPIO5 to an input so we can read the PIR output state
    Serial.begin(115200);
    Serial.println("Starting");
    client.setDebug(true); // Pass a true or false bool value to activate debug messages
    client.wifiConnection(WIFINAME, WIFIPASS);
    client.begin(callback);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    // client.ubidotsSetBroker("industrial.api.ubidots.com");
    a0Value = analogRead(A0);
    voltage = (a0Value/1023.0) * 5.0;
    client.add("voltage", voltage);
} //end setup

void loop()
{
    if (!sent)
    {
        if (!client.connected())
        {
            client.reconnect();
        }
        client.ubidotsPublish("PIR1");
        client.loop();
        sent = true;
    }
    delay(1000);// povide enough time to complete the sending of the MQTT message
    WiFi.disconnect();
    WiFi.forceSleepBegin();
    delay(10); //For some reason the modem won't go to sleep unless you do a delay(non-zero-number)
    
    //wait for PIR sense line to go low then shutdown
    if (digitalRead(pirPin) == LOW)
    {
        digitalWrite(holdPin, LOW);
    }
} // end main loop
