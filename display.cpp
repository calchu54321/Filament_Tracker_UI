#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "rotary_encoder.h"
#include <Arduino.h>


// OLED parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1   // Reset pin not used

// Create display instance using hardware I2C (SDA=21, SCL=22 by default on ESP32)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// These variables should be updated elsewhere (e.g., rotary_encoder.cpp)
String tagName = "PLA_White_1kg";
//extern String tagName; *********************LATER MAKE THIS A VARIABLE

void initDisplay() {
  Wire.begin(21, 22); // Explicitly initialize I2C with SDA=21, SCL=22
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // 0x3C is common OLED I2C address
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  drawHomeScreen();
}

void drawHomeScreen() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("Filament Tracker");

  display.setCursor(0, 16);
  display.print("Left: ");
  display.print(filamentLeft());
  display.println(" g");

  display.setCursor(0, 32);
  display.print("Used: ");
  display.print(filamentUsed());
  display.println(" g");

  display.setCursor(0, 48);
  display.print("Tag: ");
  display.println(tagName);

  display.display();
}

void drawMenu1(int selected) {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("Main Menu");

  display.setCursor(0, 16);
  display.print(selected == 0 ? "> " : "  ");
  display.println("NFC Options");

  display.setCursor(0, 32);
  display.print(selected == 1 ? "> " : "  ");
  display.println("Settings");

  display.display();
}
