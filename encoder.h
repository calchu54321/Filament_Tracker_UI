#ifndef ENCODER_H
#define ENCODER_H

void initEncoder();
long getEncoderDelta();   // Returns +1, -1, or 0 based on rotation since last check
bool isButtonPressed();   // Returns true when button is pressed

#endif
