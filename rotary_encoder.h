#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

void initRotaryEncoder();
void updateFilamentUsage();

float filamentUsed();
float filamentLeft();
long getPulseCount();

#endif
