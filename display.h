#ifndef DISPLAY_H
#define DISPLAY_H

#include "menu.h"  // To use MenuState
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

void initDisplay();
void drawHomeScreen();
void drawMenuScreen(int selected);
//Need to declare all new menu items
void Modify_NFC(int selected);
void Modify_Usage(int selected);
void Modify_Tag(int selected);
void Modify_Material_Type(int selected);
void Modify_Color(int selected);
void Modify_Spool_Weight(int selected);
void Modify_Gram_Usage(int selected);
void Reset_Menu(int selected);

#endif
