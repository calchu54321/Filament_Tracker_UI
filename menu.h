#ifndef MENU_H
#define MENU_H

enum ScreenState { HOME, MENU };

void updateMenu();
ScreenState getCurrentScreen();
int getMenuIndex();

#endif
