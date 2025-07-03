#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display; // Declare, define in display.cpp

void initDisplay();
void drawHomeScreen();
void drawMenuScreen(int selected);

#endif // DISPLAY_H
