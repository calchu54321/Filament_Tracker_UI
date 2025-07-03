#include "menu.h"
#include "encoder.h"

const int MENU_ITEMS = 5;

enum ScreenState currentScreen = HOME;
int menuIndex = 0;

void updateMenu() {
  long delta = getEncoderDelta();

  if (currentScreen == HOME) {
    if (isButtonPressed()) {
      currentScreen = MENU;
    }
  } else if (currentScreen == MENU) {
    if (delta != 0) {
      menuIndex += delta;
      if (menuIndex < 0) menuIndex = 0;
      if (menuIndex >= MENU_ITEMS) menuIndex = MENU_ITEMS - 1;
    }

    if (isButtonPressed()) {
      if (menuIndex == 0) {  // Back to Home
        currentScreen = HOME;
      }
      // Other menu items don't do anything yet
    }
  }
}

ScreenState getCurrentScreen() {
  return currentScreen;
}

int getMenuIndex() {
  return menuIndex;
}
