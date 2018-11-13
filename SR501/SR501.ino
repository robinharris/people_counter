/* This sketch reports the time between successive triggers of an HC-SR01

    12th November 2018
    Robin Harris

*/

#define pirPin 2
#define ledPin 13

//global variables
bool triggered = false;
bool currentPinState = false;
bool secondTrigger = false;
long start, end, secondTriggerStart, secondTriggerEnd;
int triggerDuration;

void setup(){
    pinMode(pirPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);//turn off the led
    Serial.begin(115200);
    Serial.println("Starting....");
}

void loop(){
    currentPinState = digitalRead(pirPin); // read the input pin state
    digitalWrite(ledPin, currentPinState); //show pin state with the LED

    if (currentPinState && !triggered){ // if the pin is high for the first time
        start = millis(); // time trigger starts
        triggered = true; // set flag to indicate in triggered state
        if (!secondTrigger){ // if this is not the second trigger start timing second trigger
            secondTriggerStart = millis();
            secondTrigger = true;
        }
        else {
            secondTriggerEnd = millis();
            secondTrigger = false;
            Serial.print("Time to second trigger: \t");
            Serial.println(secondTriggerEnd-secondTriggerStart);
            Serial.print("Blocking period: \t\t\t");
            Serial.println((secondTriggerEnd-secondTriggerStart)-triggerDuration);
        } 
    }
    if (!currentPinState && triggered){ //when the pin goes low and we are in a triggered state stop the count
        end = millis(); // time of trigger ending
        triggerDuration = end-start;
        triggered = false; // leave the triggered state
        Serial.print("Duration of trigger: \t\t");
        Serial.println(triggerDuration);
    }
}