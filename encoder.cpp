#ifndef encoder_H
#define encoder_H

#include <Arduino.h>

// Define encoder pins
const int clkPin = 32;
const int dtPin  = 35;
const int swPin  = 25;

static int lastClk = HIGH;
static int encoderDelta = 0;

static unsigned long lastDebounce = 0;
static const int debounceDelay = 50;
static bool lastButtonState = HIGH;
static bool buttonPressed = false;

void initEncoder() {
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  lastClk = digitalRead(clkPin);
}

void updateEncoder() {
  // Rotation
  int clkState = digitalRead(clkPin);
  if (clkState != lastClk && clkState == LOW) {
    if (digitalRead(dtPin) != clkState) encoderDelta++;
    else encoderDelta--;
  }
  lastClk = clkState;

  // Button debounce
  bool currentButtonState = digitalRead(swPin);
  if (currentButtonState != lastButtonState) {
    lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > debounceDelay) {
    if (lastButtonState == HIGH && currentButtonState == LOW) {
      buttonPressed = true;
    }
  }

  lastButtonState = currentButtonState;
}

int getEncoderDelta() {
  int delta = encoderDelta;
  encoderDelta = 0;
  return delta;
}

bool wasButtonPressed() {
  if (buttonPressed) {
    buttonPressed = false;
    return true;
  }
  return false;
}
#endif
