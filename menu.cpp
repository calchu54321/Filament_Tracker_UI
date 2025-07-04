#include "menu.h"
#include "encoder.h"
#include "display.h"
#include <Arduino.h>

const int MENU_ITEMS = 5;
const int MODIFY_NFC_MENU_ITEMS = 3;
const int MODIFY_TAG_MENU_ITEMS = 4;
const int MODIFY_MATERIAL_TYPE_ITEMS = 6;
const int MODIFY_COLOR_ITEMS = 6;
const int MODIFY_SPOOL_WEIGHT_ITEMS = 5;

enum MenuState currentScreen = HOME;
int menuIndex = 0;

//DEFAULT Initialize TagName
String materialType = "PLA"; //remove below 3 later
String color = "White";
String spoolWeight = "1kg";
String tagName = "PLA_White_1kg";
float spoolWeightValue = 1000; // Default weight in grams

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
    display.setCursor(0, 16);
    display.println(spoolWeight);
  }

  display.display();
  delay(2000);
  display.setTextSize(1);
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
      else if (menuIndex == 2) {  // Modify Tag
        currentScreen = MODIFY_TAG_MENU;
        menuIndex = 0;
      }
      // Other modify NFC options can be handled later
    }
  }

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
        updateTagName();
        TagChangeConfirmation();
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 2) { // 500 g
        spoolWeight = "500g";
        spoolWeightValue = 500;
        updateTagName();
        TagChangeConfirmation();
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 3) { // 1000 g
        spoolWeight = "1kg";
        spoolWeightValue = 1000;
        updateTagName();
        TagChangeConfirmation();
        currentScreen = SPOOL_WEIGHT_MENU;
        menuIndex = 0;
      }
      else if (menuIndex == 4) { // 2000 g
        spoolWeight = "2kg";
        spoolWeightValue = 2000;
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
