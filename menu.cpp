#include "menu.h"
#include "encoder.h"
#include "display.h"
#include "rotary_encoder.h" //get the filamentLeftvalue
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
    display.setCursor(0, 0);
    display.println("Material Changed To:");
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.println(materialType);
  }
  else if (currentScreen == COLOR_MENU){
    display.setCursor(0, 0);
    display.println("Color Changed To:");
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.println(color);
  }
  else if (currentScreen == SPOOL_WEIGHT_MENU){
    display.setCursor(0, 0);
    display.println("Spool Weight Changed To:");
    display.setTextSize(2);
    display.setCursor(0, 24);
    display.println(spoolWeight);
  }

  display.display();
  delay(1500);
  display.setTextSize(1);
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
      else if (menuIndex == 3) {  // Modify NFC
        currentScreen = MODIFY_NFC_MENU;
        menuIndex = 0; //Reset submenu Index
      }
      else if (menuIndex == 4) { //Reset
        materialType = "PLA"; //remove below 3 later
        color = "White";
        spoolWeight = "1kg";
        encoderCount = 0;
        spoolWeightValue = 1000; // Default weight in grams; share to rotary encoder
        filamentAdjustLeftTEMP = spoolWeightValue; // offset - spoolWeightValue
        updateTagName();

        display.clearDisplay();
        display.setCursor(10, 24);
        display.println("RESTORED DEFAULTS");
        display.display();
        delay(1500);

        currentScreen = HOME;
        menuIndex = 0;
      }
      // Other menu items don't do anything yet
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
}

MenuState getCurrentScreen() {
  return currentScreen;
}

int getMenuIndex() {
  return menuIndex;
}
