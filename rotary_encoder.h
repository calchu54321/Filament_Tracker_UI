#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>

// Rotary Encoder Setup
void initRotaryEncoder();
void updateFilamentUsage();
long getPulseCount();

// Filament Tracking Getters
float getFilamentUsed();
float getFilamentLeft();
float getLengthLeft();
float getLengthUsed();

// Filament Tracking Setters
void setFilamentUsed(float value);
void setFilamentLeft(float value);
void setLengthLeft(float value);
void setLengthUsed(float value);

extern volatile long encoderCount;


#endif
