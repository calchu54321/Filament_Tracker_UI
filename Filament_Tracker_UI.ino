#include <Arduino.h>
#include "display.h"
#include "rotary_encoder.h"
#include "encoder.h"
#include "menu.h"
#include "NFC_writer.h"

void setup() {
  Serial.begin(115200);

  initRotaryEncoder();   // Initialize rotary encoder (sets up interrupts)
  initEncoder(); //KY-040 encoder for menu navigation
  initDisplay();   // Initialize OLED display
  initNFC(); // Initialize NFC Reader/ Writer

}

void loop() {

  updateFilamentUsage();   // Update filament values first
  updateMenu(); //Update menu navigation logic

    // Draw appropriate screen based on current state
  if (getCurrentScreen() == HOME) {
    drawHomeScreen();      // Draw filament tracking screen
  } else {
    drawMenuScreen(getMenuIndex());  // Draw the menu with the current selection
  }

  //DEBUG: Show encoder pulse count in serial monitor
  // long count = getPulseCount();
  // Serial.print("Encoder pulse count: ");
  // Serial.println(count);


  delay(100);              // Faster update rate for better responsiveness
}
