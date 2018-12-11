
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
#include <Ticker.h>
#include <NTPtimeESP.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
strDateTime dateTime; // object to hold date and time from NTP
Ticker tick;          // object to be main system time keeper

NTPtime NTPuk("uk.pool.ntp.org"); // Choose server pool as required
char *ssid = "Kercem2";           // Set your WiFi SSID
char *password = "E0E3106433F4";  // Set your WiFi password
// char *ssid      = "C4Di_Members";        // Set your WiFi SSID
// char *password  = "c4d1day0ne";          // Set your WiFi password

// #define WIFINAME "workshop"
// #define WIFIPASS "workshop"

#define pirPin 2  //pin used to monitor PIR GPIO2 == D4
#define ledPin 13 //used to show when the PIR is active GPIO14 == D7
//global variables
const char *mqtt_server = "mqtt.connectedhumber.org";
const char *mqtt_user = "connectedhumber";
const char *mqtt_password = "3fds8gssf6";

bool pirActive, pirPinStatus, sentBucket;
int year, month, day, hour, minute, second = 0;
int count = 0; // number of triggers this period

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("peopleCounter", mqtt_user, mqtt_password))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 4 seconds");
      // Wait 4 seconds before retrying
      delay(5000);
    }
  }
} // end reconnect

void sendBucket()
{                            // sends the count for the last 30 minutes and resets
  char mqttBucketString[25]; // 19 for datetime, 3 for count and 1 for /0
  reconnect();               // connect to mqtt broker
  sprintf(mqttBucketString, "%4d-%2d-%2d %02d:%02d:%02d,%3d", year, month, day, hour, minute, second, count);
  mqttClient.publish("pir", mqttBucketString);
  sentBucket = true;
  count = 0;
} // end sendBucket

void timer()
{
  second++;
  if (second % 60 == 0)
  { // reset when reached 60 and increment minutes
    minute++;
    second = 0;
    if (minute % 60 == 0)
    { // reset when reached 60 and increment hours
      hour++;
      minute = 0;
    }
  }
} // end timer

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting....");
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // turn off the led
  // Connect to wifi
  // WiFiManager wifiManager;
  // wifiManager.autoConnect("PeopleCounter");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  NTPuk.setSendInterval(60);
  tick.attach(1, timer); // calls timer every second
  mqttClient.setServer(mqtt_server, 1883);
}

void loop()
{
  pirPinStatus = digitalRead(pirPin);
  if (pirPinStatus && !pirActive)
  { // if we are seeing the pirPin going high at the beginning of a trigger
    pirActive = true;
    count++;
    Serial.print("Count: \t");
    Serial.println(count);
  }
  if (!pirPinStatus && pirActive)
  { // if we see the pirPin go low and the status is ACTIVE
    pirActive = false;
  }
  digitalWrite(ledPin, pirActive); // set the led high when PIR active
  // ubidotsClient.loop();

  dateTime = NTPuk.getNTPtime(0.0, 1);
  if (dateTime.valid)
  {
    NTPuk.printDateTime(dateTime);
    year = dateTime.year;
    month = dateTime.month;
    day = dateTime.day;
    hour = dateTime.hour;
    minute = dateTime.minute;
    second = dateTime.second;
  }
  // extend this to set start and end times each day
  if ((minute == 0 || minute == 30) && !sentBucket){
    sendBucket();
  }
  if (minute != 0 && minute != 30 && sentBucket){ //don't like this test
    sentBucket = false;
  }
} //end loop
