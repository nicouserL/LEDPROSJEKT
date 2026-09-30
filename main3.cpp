#include <Arduino.h>
#include <FastLED.h>

constexpr uint8_t PIR_PIN = 2;
constexpr uint8_t LED_DATA_PIN = 3;
constexpr uint8_t BUTTON1_PIN = 5; // System on/off
constexpr uint8_t BUTTON2_PIN = 4; // Long press changes LED color
constexpr uint8_t NUM_LEDS = 5;
constexpr unsigned long DEBOUNCE_MS = 50;
constexpr unsigned long LONG_PRESS_MS = 2000;

CRGB leds[NUM_LEDS];

struct DebouncedButton {
  uint8_t pin;
  int stableState;
  int lastRawState;
  unsigned long lastChangeTime;
};

DebouncedButton button1{BUTTON1_PIN, LOW, LOW, 0};
DebouncedButton button2{BUTTON2_PIN, LOW, LOW, 0};

bool systemOn = false;
bool button2PressActive = false;
bool button2LongPressHandled = false;
bool useRed = false; // Default active color is green
unsigned long button2PressStartTime = 0;
int lastPirState = -1;

bool updateButton(DebouncedButton &button) {
  int rawState = digitalRead(button.pin);

  if (rawState != button.lastRawState) {
    button.lastRawState = rawState;
    button.lastChangeTime = millis();
  }

  if (millis() - button.lastChangeTime >= DEBOUNCE_MS &&
      rawState != button.stableState) {
    button.stableState = rawState;
    return button.stableState == HIGH; // true only on a debounced press
  }

  return false;
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  // The buttons are wired to 5V with external 10 kOhm pull-down resistors.
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  button1.stableState = button1.lastRawState = digitalRead(BUTTON1_PIN);
  button2.stableState = button2.lastRawState = digitalRead(BUTTON2_PIN);

  Serial.begin(9600);
  FastLED.addLeds<WS2812, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(160);
  FastLED.clear(true);
}

void loop() {
  if (updateButton(button1)) {
    systemOn = !systemOn;
    lastPirState = -1; // Report the PIR state again when the system is enabled.
    Serial.println(systemOn ? "System ON" : "System OFF");
  }

  if (updateButton(button2)) {
    button2PressActive = true;
    button2LongPressHandled = false;
    button2PressStartTime = millis();
  }

  if (button2PressActive && button2.stableState == HIGH &&
      !button2LongPressHandled &&
      millis() - button2PressStartTime >= LONG_PRESS_MS) {
    useRed = !useRed;
    button2LongPressHandled = true;
    Serial.println(useRed ? "Long press: red" : "Long press: green");
  }

  if (button2PressActive && button2.stableState == LOW) {
    button2PressActive = false;
  }

  CRGB outputColor = CRGB::Black;
  if (systemOn) {
    int pirState = digitalRead(PIR_PIN);
    if (pirState != lastPirState) {
      Serial.println(pirState == HIGH ? "PIR motion detected" : "No PIR motion");
      lastPirState = pirState;
    }

    if (pirState == HIGH) {
      outputColor = useRed ? CRGB::Red : CRGB::Green;
    }
  }

  for (uint8_t i = 0; i < NUM_LEDS; ++i) {
    leds[i] = outputColor;
  }
  FastLED.show();
}
