#include "menu.h"
#include "encoder.h"
#include "display.h"
#include "rotary_encoder.h" //get the filamentLeftvalue
#include "NFC_writer.h"
#include <Arduino.h>

const int MENU_ITEMS = 5;
const int MODIFY_NFC_MENU_ITEMS = 3;
const int MODIFY_TAG_MENU_ITEMS = 4;
const int MODIFY_MATERIAL_TYPE_ITEMS = 6;
const int MODIFY_COLOR_ITEMS = 6;
const int MODIFY_SPOOL_WEIGHT_ITEMS = 5;
const int MODIFY_USAGE_ITEMS = 5;
const int ADJUST_GRAMS_LEFT_ITEMS = 6;
const int ADJUST_GRAMS_USED_ITEMS = 6;
const int RESET_MENU_ITEMS = 2;

enum MenuState currentScreen = HOME;
int menuIndex = 0;

//DEFAULT Initialize TagName
String materialType = "PLA"; //remove below 3 later
String color = "White";
String spoolWeight = "1kg";
String tagName = "PLA_White_1kg";
extern float spoolWeightValue = 1000; // Default weight in grams; share to rotary encoder
extern float filamentAdjustLeftTEMP = spoolWeightValue; // offset - spoolWeightValue

//Adjustable Tag Name 
void updateTagName(){
  tagName = materialType + "_" + color + "_" + spoolWeight;
}

void TagChangeConfirmation(){
  // Show confirmation message of material chnage
  display.clearDisplay();
  if (currentScreen == MATERIAL_TYPE_MENU){
    printCenteredText("Material Changed To:", 0, 1);
    printCenteredText(materialType, 24, 2);
  }
  else if (currentScreen == COLOR_MENU){
    printCenteredText("Color Changed To:", 0, 1);
    printCenteredText(color, 24, 2);
  }
  else if (currentScreen == SPOOL_WEIGHT_MENU){
    printCenteredText("Spool Weight Changed To:", 0, 1);
    printCenteredText(spoolWeight, 24, 2);
  }
  display.display();
  delay(1500);
  display.setTextSize(1);
}

//Centered Text
//EX: printCenteredText("Hello World", 24, 2); string, y position, textsize
void printCenteredText(String text, int y, int textSize = 1) {
  display.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;

  display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;  // SCREEN_WIDTH = 128 for most OLEDs

  display.setCursor(x, y);
  display.println(text);
}


//Selecting Tolerance
float selectedTolerance = 1.0;  // Holds chosen adjustment step

void ModifyUsageValues(){
  bool adjusting = true;
  float valueToAdjust = 0.0;
  float valueTemp = 0.0;

  MenuState previousScreen = currentScreen;

  if (currentScreen == ADJUST_GRAMS_LEFT_MENU){
    encoderCount = 0; // reset rotary encoder count
    valueToAdjust = getFilamentLeft();
  }
  else if (currentScreen == ADJUST_GRAMS_USED_MENU){
    encoderCount = 0; // reset rotary encoder count
    valueToAdjust = getFilamentUsed();
    valueTemp = getFilamentLeft();
  }
  else if (currentScreen == ADJUST_LENGTH_LEFT_MENU){
    encoderCount = 0; // reset rotary encoder count
    valueToAdjust = getLengthLeft();
  }
  else if (currentScreen == ADJUST_LENGTH_USED_MENU){
    encoderCount = 0; // reset rotary encoder count
    valueToAdjust = getLengthUsed();
  }

  while (adjusting){
    long delta = getEncoderDelta();
    valueToAdjust += delta * selectedTolerance;

    display.clearDisplay();
    display.setCursor(0, 0);
    if (currentScreen == ADJUST_GRAMS_LEFT_MENU) {
      display.println("Adjusted Filament ");
      display.setCursor(0, 8);
      display.println("Left:");
      display.setCursor(0, 24);
      display.setTextSize(2);
      display.print(valueToAdjust, 2);
      display.println("g");
    } else if (currentScreen == ADJUST_GRAMS_USED_MENU) {
      display.println("Adjusted Filament ");
      display.setCursor(0, 8);
      display.println("Used:");
      display.setCursor(0, 24);
      display.setTextSize(2);
      display.print(valueToAdjust, 2);
      display.println("g");
    } else if (currentScreen == ADJUST_LENGTH_LEFT_MENU) {
      display.println("Adjusted Length Left:");
      display.setCursor(0, 24);
      display.setTextSize(2);
      display.print(valueToAdjust, 2);
      display.println("mm");
    } else if (currentScreen == ADJUST_LENGTH_USED_MENU) {
      display.println("Adjusted Length Used:");
      display.setCursor(0, 24);
      display.setTextSize(2);
      display.print(valueToAdjust, 2);
      display.println("mm");
    }
    display.display();
    display.setTextSize(1);

    if (isButtonPressed()) {
      adjusting = false;
      display.setCursor(0, 48);
      display.print("Saved ");
      display.display();
      delay(1500);
    }
  }
  
  // Save adjusted value back
  if (previousScreen == ADJUST_GRAMS_LEFT_MENU) {
    filamentAdjustLeftTEMP = valueToAdjust;
    currentScreen = ADJUST_GRAMS_LEFT_MENU;
    menuIndex = 0;
  } else if (previousScreen == ADJUST_GRAMS_USED_MENU) {
    filamentAdjustLeftTEMP = spoolWeightValue - valueToAdjust;
    currentScreen = ADJUST_GRAMS_USED_MENU;
    menuIndex = 0;
  } else if (previousScreen == ADJUST_LENGTH_LEFT_MENU) {
    setLengthLeft(valueToAdjust); //need to change
  } else if (previousScreen == ADJUST_LENGTH_USED_MENU) {
    setLengthUsed(valueToAdjust); //need to change
  }

}

void updateMenu() {
  long delta = getEncoderDelta();

  if (currentScreen == HOME) {
    if (isButtonPressed()) {
      currentScreen = MAIN_MENU;
    }
  } 
  else if (currentScreen == MAIN_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MENU_ITEMS) menuIndex = MENU_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to Home
        currentScreen = HOME;
      }
      else if (menuIndex == 1) { // Export to NFC
        display.clearDisplay();
        printCenteredText("Place NFC", 10, 2);
        printCenteredText("on Reader", 36, 2);
        display.display();
        display.setTextSize(1);

        bool tagDetected = false;
        bool success = false;
        bool aborted = false;
        unsigned long startTime = millis();
        const unsigned long timeout = 10000; // 10 seconds timeoue

        // Wait for NFC tag or timeout
        while (millis() - startTime < timeout) {
          if(isTagPresentCustom()) {
            tagDetected = true;
            break;
          }
          if (isButtonPressed()) {
            aborted = true;
            break;
          }
          delay(10);
        }
        if (tagDetected) {
          success = writeFilamentData(
          materialType,
          color,
          spoolWeight,
          tagName,
          spoolWeightValue,
          getFilamentUsed(),
          getFilamentLeft()
          );
        }
        display.clearDisplay();
        display.setCursor(0,0);
        if (tagDetected && success) {
          printCenteredText("Exported", 10, 2);
          printCenteredText("To NFC", 36, 2);
        }
        else if (aborted) {
          printCenteredText("Aborted", 10, 2);
        }
        else if (!tagDetected){
          printCenteredText("Timed Out", 10, 2);
          display.display();
          delay(1000);
          display.clearDisplay();
          printCenteredText("No Tag", 10, 2);
          printCenteredText("Found", 36, 2);
        }
        display.setTextSize(1);
        display.display();
        delay(1500);
        menuIndex = 0;
      }
      else if (menuIndex == 2) { // Import from NFC
        display.clearDisplay();
        printCenteredText("Place NFC", 10, 2);
        printCenteredText("on Reader", 36, 2);
        display.display();
        display.setTextSize(1);

        bool tagDetected = false;
        bool success = false;
        bool aborted = false;
        unsigned long startTime = millis();
        const unsigned long timeout = 10000; // 10 seconds timeout

        //wait for NFC tag or timeout or abort
        while (millis() - startTime < timeout) {
          if (isTagPresentCustom()) {
            tagDetected = true;
            break;
            delay(100);
          }
          if (isButtonPressed()) {
            aborted = true;
            break;
          }
          delay (100);
        }
        if (tagDetected) {
          success = readFilamentData(
            materialType,
            color,
            spoolWeight,
            tagName,
            spoolWeightValue,
            filamentAdjustLeftTEMP,
            pulses
          );
        }
        display.clearDisplay();
        display.setCursor(0,0);
        if (tagDetected && success) {
          printCenteredText("Imported", 10, 2);
          printCenteredText("From NFC", 36, 2);
        }
        else if (aborted) {
          printCenteredText("Aborted", 10, 2);
        }
        else if (!tagDetected) {
          printCenteredText("Timed Out", 10, 2);
          display.display();
          delay(1000);
          display.clearDisplay();
          printCenteredText("No Tag", 10, 2);
          printCenteredText("Found", 36, 2);
        }
        display.setTextSize(1);
        display.display();
        delay(1500);
        menuIndex = 0;
      }
      else if (menuIndex == 3) {  // Modify NFC
        currentScreen = MODIFY_NFC_MENU;
        menuIndex = 0; //Reset submenu Index
      }
      else if (menuIndex == 4) { // Reset
        currentScreen = RESET_MENU;
        menuIndex = 0;
      }
    }
  }
  
  //MODIFY_NFC SUBMENU
  else if (currentScreen == MODIFY_NFC_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MODIFY_NFC_MENU_ITEMS) menuIndex = MODIFY_NFC_MENU_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to Main Menu
        currentScreen = MAIN_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) { // Modify Usage
        currentScreen = MODIFY_USAGE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 2) {  // Modify Tag
        currentScreen = MODIFY_TAG_MENU;
        menuIndex = 0;
      }
    }
  }

  //MODIFY_USAGE SUBMENU
  else if (currentScreen == MODIFY_USAGE_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MODIFY_USAGE_ITEMS) menuIndex = MODIFY_USAGE_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to Modify NFC menu
        currentScreen = MODIFY_NFC_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) { // Modify grams Left
        currentScreen = ADJUST_GRAMS_LEFT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 2) {  // Modify grams Used
        currentScreen = ADJUST_GRAMS_USED_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 3){ // Modify Length Left
        currentScreen = ADJUST_LENGTH_LEFT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 4){ // Modify Length Used
        currentScreen = ADJUST_LENGTH_USED_MENU;
        menuIndex = 0;
      }
    }
  }

  //Adjust_Grams_Left SUBMENU
  else if (currentScreen == ADJUST_GRAMS_LEFT_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= ADJUST_GRAMS_LEFT_ITEMS) menuIndex = ADJUST_GRAMS_LEFT_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to Modify Usage menu
        currentScreen = MODIFY_USAGE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) { // 0.01 g
        selectedTolerance = 0.01;
        ModifyUsageValues();
      }
      else if (menuIndex == 2) { // 0.1 g
        selectedTolerance = 0.1;
        ModifyUsageValues();
      }
      else if (menuIndex == 3) { // 1 g
        selectedTolerance = 1;
        ModifyUsageValues();
      }
      else if (menuIndex == 4) { // 10 g
        selectedTolerance = 10;
        ModifyUsageValues();
      }
      else if (menuIndex == 5) { // 100 g
        selectedTolerance = 100;
        ModifyUsageValues();
      }
    }

  }

  //Adjust_Grams_USED SUBMENU
  else if (currentScreen == ADJUST_GRAMS_USED_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= ADJUST_GRAMS_LEFT_ITEMS) menuIndex = ADJUST_GRAMS_LEFT_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to Modify Usage menu
        currentScreen = MODIFY_USAGE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) { // 0.01 g
        selectedTolerance = 0.01;
        ModifyUsageValues();
      }
      else if (menuIndex == 2) { // 0.1 g
        selectedTolerance = 0.1;
        ModifyUsageValues();
      }
      else if (menuIndex == 3) { // 1 g
        selectedTolerance = 1;
        ModifyUsageValues();
      }
      else if (menuIndex == 4) { // 10 g
        selectedTolerance = 10;
        ModifyUsageValues();
      }
      else if (menuIndex == 5) { // 100 g
        selectedTolerance = 100;
        ModifyUsageValues();
      }
    }

  }
  //////////////////////////////////////////////////////////////////////////////////////
  //MODIFY_TAG SUBMENU
  else if (currentScreen == MODIFY_TAG_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MODIFY_TAG_MENU_ITEMS) menuIndex = MODIFY_TAG_MENU_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to MODIFY_NFC_MENU
        currentScreen = MODIFY_NFC_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1){ // Material Type
        currentScreen = MATERIAL_TYPE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 2){ // Color
        currentScreen = COLOR_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 3){ // Spool Weight
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
    }
  }

  //MODIFY_MATERIAL TYPE SUBMENU
  else if (currentScreen == MATERIAL_TYPE_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MODIFY_MATERIAL_TYPE_ITEMS) menuIndex = MODIFY_MATERIAL_TYPE_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to MODIFY_TAG
        currentScreen = MODIFY_TAG_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) {  // PLA
        materialType = "PLA";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = MATERIAL_TYPE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 2) { //ABS
        materialType = "ABS";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = MATERIAL_TYPE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 3) { //PETG
        materialType = "PETG";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = MATERIAL_TYPE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 4) { //TPU
        materialType = "TPU";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = MATERIAL_TYPE_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 5) { //Nylon
        materialType = "Nylon";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = MATERIAL_TYPE_MENU;
        menuIndex = 0;
      }
    }
  }

  //COLOR SUBMENU
  else if (currentScreen == COLOR_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MODIFY_COLOR_ITEMS) menuIndex = MODIFY_COLOR_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to MODIFY_TAG
        currentScreen = MODIFY_TAG_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) {  // Black
        color = "Black";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = COLOR_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 2) { // White
        color = "White";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = COLOR_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 3) { // Red
        color = "Red";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = COLOR_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 4) { // Blue
        color = "Blue";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = COLOR_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 5) { // Green
        color = "Green";
        updateTagName();
        TagChangeConfirmation();
        currentScreen = COLOR_MENU;
        menuIndex = 0;
      }
    }
  }

  //SPOOL_WEIGHT SUBMENU
  else if (currentScreen == SPOOL_WEIGHT_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MODIFY_SPOOL_WEIGHT_ITEMS) menuIndex = MODIFY_SPOOL_WEIGHT_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to MODIFY_TAG
        currentScreen = MODIFY_TAG_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) {  // 250 g
        spoolWeight = "250g";
        spoolWeightValue = 250;
        filamentAdjustLeftTEMP = spoolWeightValue;
        updateTagName();
        TagChangeConfirmation();
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 2) { // 500 g
        spoolWeight = "500g";
        spoolWeightValue = 500;
        filamentAdjustLeftTEMP = spoolWeightValue;
        updateTagName();
        TagChangeConfirmation();
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 3) { // 1000 g
        spoolWeight = "1kg";
        spoolWeightValue = 1000;
        filamentAdjustLeftTEMP = spoolWeightValue;
        updateTagName();
        TagChangeConfirmation();
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 4) { // 2000 g
        spoolWeight = "2kg";
        spoolWeightValue = 2000;
        filamentAdjustLeftTEMP = spoolWeightValue;
        updateTagName();
        TagChangeConfirmation();
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
    }
  }
    //RESET SUBMENU
  else if (currentScreen == RESET_MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= RESET_MENU_ITEMS) menuIndex = RESET_MENU_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // No
        currentScreen = MAIN_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 1) { // Yes
        // materialType = "PLA";
        // color = "White";
        // spoolWeight = "1kg";
        // encoderCount = 0;
        // spoolWeightValue = 1000; // Default weight in grams; share to rotary encoder
        // filamentAdjustLeftTEMP = spoolWeightValue; // offset - spoolWeightValue
        // updateTagName();

        // display.clearDisplay();
        // printCenteredText("RESTORED DEFAULTS", 24, 1);
        // display.display();
        // delay(1500);

        // currentScreen = HOME;
        // menuIndex = 0;
        ESP.restart();
      }
    }
  }
}

MenuState getCurrentScreen() {
  return currentScreen;
}

int getMenuIndex() {
  return menuIndex;
}
