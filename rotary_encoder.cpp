#include "rotary_encoder.h"
#include <Arduino.h>

// Pin definitions
#define ENCODER_A 18
#define ENCODER_B 19

volatile long encoderCount = 0;
volatile int lastEncoded = 0;

// Constants
const float pulsesPerMM = 600.0 / 100.0;    // 600 P/R per 100mm
const float gramsPerMM = 1000.0 / 33000.0;  // 1kg = 33m → 33000mm

// Filament state
static float filamentUsedValue = 0.0;
static float filamentLeftValue = 1000.0;

void IRAM_ATTR updateRotaryEncoder() {
  int MSB = digitalRead(ENCODER_A);
  int LSB = digitalRead(ENCODER_B);
  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    encoderCount--;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
    encoderCount++;

  lastEncoded = encoded;
}

void initRotaryEncoder() {
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  // Read initial state
  int MSB = digitalRead(ENCODER_A);
  int LSB = digitalRead(ENCODER_B);
  lastEncoded = (MSB << 1) | LSB;

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateRotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), updateRotaryEncoder, CHANGE);
}

void updateFilamentUsage() {
  noInterrupts();
  long pulses = encoderCount;
  interrupts();

  float mmUsed = pulses / pulsesPerMM;
  filamentUsedValue = mmUsed * gramsPerMM;
  filamentLeftValue = 1000.0 - filamentUsedValue;
}

float filamentUsed() {
  return filamentUsedValue;
}

float filamentLeft() {
  return filamentLeftValue;
}

long getPulseCount() {
  noInterrupts();
  long p = encoderCount;
  interrupts();
  return p;
}
