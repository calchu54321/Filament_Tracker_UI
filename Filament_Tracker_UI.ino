#include <Arduino.h>
#include "display.h"
#include "rotary_encoder.h"

// To keep track of menu and encoder position
int menuSelection = 0;

void setup() {
  Serial.begin(115200);

  initRotaryEncoder();   // Initialize rotary encoder (sets up interrupts)
  initDisplay();   // Initialize OLED display

}

void loop() {

  updateFilamentUsage();   // Update filament values first

  //DEBUG: Show encoder pulse count in serial monitor
  long count = getPulseCount();
  Serial.print("Encoder pulse count: ");
  Serial.println(count);

  drawHomeScreen();        // Then draw the screen with fresh values


  delay(500);              // Faster update rate for better responsiveness
}
