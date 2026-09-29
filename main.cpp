#include <Arduino.h>
#include <FastLED.h>
#define NUM_LEDS 5
CRGB leds[NUM_LEDS];


const int buttonPin = 2;  // the pin that the button is connected to

int buttonState = LOW;      // the debounced state of the input pin
int lastButtonState = LOW;  // the previous reading from the input pin
unsigned long pressStartTime = 0;   // the time when the button was pressed
unsigned long pressDuration = 0;    // the duration of the button press

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time (endre hvis knappen er ustabil)

void setup() {
  Serial.begin(9600);               //Serial Baudrate
  pinMode(buttonPin, INPUT);     // initialize the button pin as an input:
  FastLED.addLeds<WS2812, 13>(leds, NUM_LEDS);     // initialize the LED pin as an output with FastLED
  FastLED.setBrightness(160);       //Set brightness with FastLED

}

void loop() {
  int reading = digitalRead(buttonPin); //read the state of the button, saved in a variable
  if (reading != lastButtonState) { //if statement for when the button state changes, saves the time in a variable
    lastDebounceTime = millis(); //starts the timer and saves the time in a variable
  }

  if ((millis() - lastDebounceTime) > debounceDelay) { 
    //checks if the button state has been stable for longer than the debounce delay

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // The external resistor pulls the input LOW until the button is pressed.
      if (buttonState == HIGH) {
        pressStartTime = millis(); //start timing after the press is debounced
      } else {
        pressDuration = millis() - pressStartTime; //measure when the release is debounced
        Serial.println(pressDuration);
        Serial.print("Du har holdt knappen i ");
        Serial.print(pressDuration / 1000);
        Serial.println(" Sekunder");

        if (pressDuration > 2000) { //button was held for more than 2 seconds
          Serial.println("Du har holdt knappen i mere enn 2 sekunder");
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Red;
          }
        } else { //button was held for 2 seconds or less
          Serial.println("Du har holdt knappen i 2 sekunder eller mindre");
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Green;
          }
        }
        FastLED.show();
      }
      }
    }
  
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  
}