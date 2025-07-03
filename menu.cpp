#include "menu.h"
#include "encoder.h"

const int MENU_ITEMS = 5;
const int MODIFY_NFC_MENU_ITEMS =3;

enum MenuState currentScreen = HOME;
int menuIndex = 0;

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
      // Other modify NFC options can be handled later
    }
  }
}

MenuState getCurrentScreen() {
  return currentScreen;
}

int getMenuIndex() {
  return menuIndex;
}
