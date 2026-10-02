//Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Arduino.h>
#include "RTClib.h"
#include <FastLED.h>

#define NUM_LEDS 5
#define COLOR_ORDER GRB;

const int LED_PIN = 3;


RTC_DS3231 rtc;
CRGB leds[NUM_LEDS];

void setup () {
  Serial.begin(9600);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(160);
  FastLED.clear(true);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  }


void loop () {

    int temp = rtc.getTemperature();

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" C");

    Serial.println();
    delay(3000);

    if(temp>27){
        for (int i = 0; i<NUM_LEDS; i++){
            leds[i] = CRGB::Red;
        }
    }
    else if (temp==27){
        for (int i = 0; i<NUM_LEDS; i++){
            leds[i] = CRGB::Yellow;
        }
    }
    else{
        for (int i = 0; i<NUM_LEDS; i++){
            leds[i] = CRGB::Blue;
        }
    }
    FastLED.show();
}