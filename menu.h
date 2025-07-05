#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
// Define all screen states
enum MenuState {
  HOME,
  MAIN_MENU,
  MODIFY_NFC_MENU,
  MODIFY_USAGE_MENU,
  ADJUST_GRAMS_LEFT_MENU,
  ADJUST_GRAMS_USED_MENU,
  ADJUST_LENGTH_LEFT_MENU,
  ADJUST_LENGTH_USED_MENU,
  MODIFY_TAG_MENU,
  MATERIAL_TYPE_MENU,
  COLOR_MENU,
  SPOOL_WEIGHT_MENU
};

// Function declarations
void updateMenu();
MenuState getCurrentScreen();
int getMenuIndex();
void updateTagName();

extern String tagName;
extern float spoolWeightValue; //share to rotary_encoder.cpp 
extern float filamentAdjustLeftTEMP; //share to rotary_encoder.cpp

#endif
