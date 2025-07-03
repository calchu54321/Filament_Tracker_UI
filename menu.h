#ifndef MENU_H
#define MENU_H

// Define all screen states
enum MenuState {
  HOME,
  MAIN_MENU,
  MODIFY_NFC_MENU
};

// Function declarations
void updateMenu();
MenuState getCurrentScreen();
int getMenuIndex();

#endif
