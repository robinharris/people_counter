/*
   This sketch shows an example of sending a reading to data.sparkfun.com once per day.

   It uses the Sparkfun testing stream so the only customizing required is the WiFi SSID and password.

   The Harringay Maker Space
   License: Apache License v2
*/
#include <NTPtimeESP.h>

#define DEBUG_ON


NTPtime NTPuk("uk.pool.ntp.org");   // Choose server pool as required
char *ssid      = "Kercem2";               // Set you WiFi SSID
char *password  = "E0E3106433F4";               // Set you WiFi password

/*
 * The structure contains following fields:
 * struct strDateTime
{
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte dayofWeek;
  boolean valid;
};
 */
strDateTime dateTime;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");
  Serial.println("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
  NTPuk.setSendInterval(10);
}


void loop() {

  // first parameter: Time zone in floating point (for India); second parameter: 1 for European summer time; 2 for US daylight saving time; 0 for no DST adjustment; (contributed by viewwer, not tested by me)

  // check dateTime.valid before using the returned time
  // Use "setSendInterval" or "setRecvTimeout" if required
  dateTime = NTPuk.getNTPtime(0.0, 1);
  // if (dateTime.valid){
    NTPuk.printDateTime(dateTime);

    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualsecond = dateTime.second;
    int actualyear = dateTime.year;
    byte actualMonth = dateTime.month;
    byte actualday =dateTime.day;
    byte actualdayofWeek = dateTime.dayofWeek;
    Serial.print("Minutes: \t");
    Serial.println(actualMinute);
  // }
  delay(10000);
}
