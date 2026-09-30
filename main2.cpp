#include <Arduino.h> 
#include <FastLED.h>

int sensorState = 0;
int lastSensorState = -1; // remembers the last PIR state printed to Serial

int pirPin = 2;
int button1Pin = 5; //OnOffbuttonpin
int button1State = LOW;
int lastButton1State = LOW;  //previous raw button state (released with pull-down)
int button1Reading = LOW;    //debounced button state
unsigned long pressStartB1 = 0; //the time when button1/OnOff button was pressed
unsigned long pressDurationB1 = 0; //Duration of button1/OnOff button press

unsigned long lastDebounceTime1 = 0; //the last time the output pin was toggled
unsigned long debounceDelay1 = 50; //debounce delay time (endre hvis knappen er ustabil)

#define NUM_LEDS 5



#ifndef PIN_DATA
#define PIN_DATA 3
#endif  // PIN_DATA

CRGB leds[NUM_LEDS];
#define COLOR_ORDER GRB

void setup()
{
  pinMode(button1Pin, INPUT);
  pinMode(pirPin, INPUT); //set pinmode for PIR sensor
  Serial.begin(9600);
  FastLED.addLeds<WS2812, PIN_DATA, COLOR_ORDER>(leds, NUM_LEDS); //initialize the Led pin as output with FastLED
  FastLED.setBrightness(160); //set LED Brightness

  

}

void loop()
{
  int reading1 = digitalRead(button1Pin); //read state of OnOff button
  if (reading1 != lastButton1State ){
    lastDebounceTime1 = millis();

  }

  
  if ((millis() - lastDebounceTime1) > debounceDelay1){
    
      if (reading1 != button1Reading){
        button1Reading = reading1;
      
        if (button1Reading == HIGH){ //HIGH means pressed with the pull-down wiring

            button1State = !button1State;

            Serial.print("Button State: ");
            Serial.println(button1State);
        }
      }
  }

  lastButton1State = reading1;


  if (button1State == HIGH) {  
    sensorState = digitalRead(pirPin);

    if (sensorState != lastSensorState) {
      Serial.println(sensorState == HIGH ? "SENSOR PÅ" : "SENSOR AV");
      lastSensorState = sensorState;
    }

    if (sensorState == HIGH) { //pir sensor on
      for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB::Green;
      }
    } else { //pir sensor off
      for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB::Red;
      }
    }
  } else { //button system off
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
}