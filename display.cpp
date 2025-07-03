#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "rotary_encoder.h"
#include "menu.h"
#include <Arduino.h>


// OLED parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1   // Reset pin not used

// Create display instance using hardware I2C (SDA=21, SCL=22 by default on ESP32)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Global menu state (defined here)
extern MenuState currentScreen;

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
  display.print(filamentLeft()); //filamentLeft from rotary_encoder.h
  display.println(" g");

  display.setCursor(0, 32);
  display.print("Used: ");
  display.print(filamentUsed()); //filamentUsed from rotary_encoder.h
  display.println(" g");

  display.setCursor(0, 48);
  display.print("Tag: ");
  display.println(tagName);

  display.display();
}

#define MENU_ITEMS 6
const char* menuItems[MENU_ITEMS] = {
  "Back to Home",
  "Store to NFC",
  "Import from NFC",
  "Modify NFC",
  "Reset",
  "" //need to add a dummy item to prvent visual bug
};


void drawMenuScreen(int selected) {
  if (currentScreen == MAIN_MENU) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Menu");

    int startItem = selected - 1;
    if (startItem < 0) startItem = 0;
    if (startItem > MENU_ITEMS - 3) startItem = MENU_ITEMS - 3;

    for (int i = 0; i < 3; i++) {
      int itemIndex = startItem + i;
      if (itemIndex >= MENU_ITEMS) break;

      if (itemIndex == selected) {
        display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }

      display.setCursor(0, 16 * (i + 1));
      display.println(menuItems[itemIndex]);
    }

    display.display();
     
  }
  //Modify_NFC_MENU 
  else if (currentScreen == MODIFY_NFC_MENU) {
    Modify_NFC(selected);
  }
}


//Modify_NFC Submenu
void Modify_NFC(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Modify NFC");

  const int ModifyNFCItemsCount = 4;
  const char* ModifyNFCItems[ModifyNFCItemsCount] = {
    "Back",
    "Modify Usage",
    "Modify Tag",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ModifyNFCItemsCount - 3) startItem = ModifyNFCItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ModifyNFCItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ModifyNFCItems[itemIndex]);
  }

  display.display();
}

