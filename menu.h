#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
// Define all screen states
enum MenuState {
  HOME,
  MAIN_MENU,
  MODIFY_NFC_MENU,
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
extern float spoolWeightValue;

#endif
