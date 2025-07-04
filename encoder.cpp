#include "encoder.h"
#include <Arduino.h>

// KY-040 Pins
#define ENCODER_CLK 32
#define ENCODER_DT 35
#define ENCODER_SW 25  // Change to your button pin

// Internal state
volatile int lastClkState = 0;
volatile int encoderDelta = 0;
volatile bool buttonPressed = false;

// Debounce state
volatile unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 200;  // Debounce delay in ms

void IRAM_ATTR handleEncoder() {
  int clkState = digitalRead(ENCODER_CLK);
  int dtState = digitalRead(ENCODER_DT);

  if (clkState != lastClkState) {
    if (dtState != clkState) {
      encoderDelta++;
    } else {
      encoderDelta--;
    }
  }
  lastClkState = clkState;
}

void IRAM_ATTR handleButton() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonPressTime > debounceDelay) {
    buttonPressed = true;
    lastButtonPressTime = currentTime;
  }
}

void initEncoder() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  lastClkState = digitalRead(ENCODER_CLK);

  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), handleButton, FALLING);
}

long getEncoderDelta() {
  noInterrupts();
  long delta = encoderDelta;
  encoderDelta = 0;
  interrupts();
  return delta;
}

bool isButtonPressed() {
  noInterrupts();
  bool pressed = buttonPressed;
  buttonPressed = false;
  interrupts();
  return pressed;
}
