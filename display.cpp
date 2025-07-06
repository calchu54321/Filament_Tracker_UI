#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "rotary_encoder.h"
#include "menu.h"
#include <Arduino.h>


// OLED parameters
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET    = -1;   // Reset pin not used

// Create display instance using hardware I2C (SDA=21, SCL=22 by default on ESP32)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Global menu state (defined here)
extern MenuState currentScreen;


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
  display.print(getFilamentLeft()); //filamentLeft from rotary_encoder.h
  display.println(" g");

  display.setCursor(0, 32);
  display.print("Used: ");
  display.print(getFilamentUsed()); //filamentUsed from rotary_encoder.h
  display.println(" g");

  display.setCursor(0, 48);
  display.print("Tag: ");
  display.println(tagName);

  display.display();
}

#define MENU_ITEMS 6
const char* menuItems[MENU_ITEMS] = {
  "Back to Home",
  "Export to NFC",
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
  //Modify_Tag_Menu
  else if (currentScreen == MODIFY_TAG_MENU) {
    Modify_Tag(selected);
  }
  //Material_Type_Menu
  else if (currentScreen == MATERIAL_TYPE_MENU) {
    Modify_Material_Type(selected);
  }
  //Color_menu
  else if (currentScreen == COLOR_MENU){
    Modify_Color(selected);
  }
  else if (currentScreen == SPOOL_WEIGHT_MENU){
    Modify_Spool_Weight(selected);
  }
  else if (currentScreen == MODIFY_USAGE_MENU){
    Modify_Usage(selected);
  }
  else if (currentScreen == ADJUST_GRAMS_LEFT_MENU){
    Modify_Gram_Usage(selected);
  }
  else if (currentScreen == ADJUST_GRAMS_USED_MENU) {
    Modify_Gram_Usage(selected);
  }
  else if (currentScreen == RESET_MENU) {
    Reset_Menu(selected);
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

//Modify_Tag Submenu
void Modify_Tag(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Modify Tag");

  const int ModifyTagItemsCount = 5;
  const char* ModifyTagItems[ModifyTagItemsCount] = {
    "Back",
    "Material Type",
    "Color",
    "Spool Weight",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ModifyTagItemsCount - 3) startItem = ModifyTagItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ModifyTagItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ModifyTagItems[itemIndex]);
  }

  display.display();
}

//Modify_Material_type Submenu
void Modify_Material_Type(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Material Type");

  const int ModifyMaterialTypeItemsCount = 7;
  const char* ModifyMaterialTypeItems[ModifyMaterialTypeItemsCount] = {
    "Back",
    "PLA",
    "ABS",
    "PETG",
    "TPU",
    "Nylon",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ModifyMaterialTypeItemsCount - 3) startItem = ModifyMaterialTypeItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ModifyMaterialTypeItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ModifyMaterialTypeItems[itemIndex]);
  }

  display.display();
}

//Modify_Color Submenu
void Modify_Color(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Color");

  const int ModifyColorItemsCount = 7;
  const char* ModifyColorItems[ModifyColorItemsCount] = {
    "Back",
    "Black",
    "White",
    "Red",
    "Blue",
    "Green",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ModifyColorItemsCount - 3) startItem = ModifyColorItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ModifyColorItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ModifyColorItems[itemIndex]);
  }

  display.display();
}

//Modify_Spool_Weight Submenu
void Modify_Spool_Weight(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Color");

  const int ModifySpoolWeightItemsCount = 6;
  const char* ModifySpoolWeightItems[ModifySpoolWeightItemsCount] = {
    "Back",
    "250 g",
    "500 g",
    "1 kg",
    "2 kg",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ModifySpoolWeightItemsCount - 3) startItem = ModifySpoolWeightItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ModifySpoolWeightItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ModifySpoolWeightItems[itemIndex]);
  }

  display.display();
}

//Modify_Usage Submenu
void Modify_Usage(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Modify Usage");

  const int ModifyUsageItemsCount = 6;
  const char* ModifyUsageItems[ModifyUsageItemsCount] = {
    "Back",
    "Adjust Grams Left",
    "Adjust Grams Used",
    "Adjust Length Left",
    "Adjust Length Used",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ModifyUsageItemsCount - 3) startItem = ModifyUsageItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ModifyUsageItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ModifyUsageItems[itemIndex]);
  }

  display.display();
  
}

//Modify_Gram_Usage Submenu
void Modify_Gram_Usage(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Select A Tolerance");

  const int ModifyGramUsageItemsCount = 7;
  const char* ModifyGramUsageItems[ModifyGramUsageItemsCount] = {
    "Back",
    "+/- 0.01 g",
    "+/- 0.1 g",
    "+/- 1 g",
    "+/- 10 g",
    "+/- 100 g",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ModifyGramUsageItemsCount - 3) startItem = ModifyGramUsageItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ModifyGramUsageItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ModifyGramUsageItems[itemIndex]);
  }

  display.display();
  
}
 
// Reset Submenu
void Reset_Menu(int selected) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Restore Defaults?");

  const int ResetItemsCount = 3;
  const char* ResetItems[ResetItemsCount] = {
    "No",
    "Yes",
    "" //need to add a dummy item to prvent visual bug
  };

  int startItem = selected - 1;
  if (startItem < 0) startItem = 0;
  if (startItem > ResetItemsCount - 3) startItem = ResetItemsCount - 3;

  for (int i = 0; i < 3; i++) {
    int itemIndex = startItem + i;
    if (itemIndex >= ResetItemsCount) break;

    if (itemIndex == selected) {
      display.fillRect(0, 16 * (i + 1) - 2, SCREEN_WIDTH, 14, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(0, 16 * (i + 1));
    display.println(ResetItems[itemIndex]);
  }

  display.display();
  
}
