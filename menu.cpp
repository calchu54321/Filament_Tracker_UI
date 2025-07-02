#include <Arduino.h>
#include "display.h"  // Directly include if you're not using headers

enum Page { HOME, MENU_1 };
Page currentPage = HOME;

int selectedOption = 0;
const int totalOptions = 2;

void handleMenuInput(int delta) {
  if (currentPage == MENU_1) {
    selectedOption += delta;
    if (selectedOption < 0) selectedOption = totalOptions - 1;
    if (selectedOption >= totalOptions) selectedOption = 0;
    drawMenu1(selectedOption);
  }
}

void handleButtonPress() {
  if (currentPage == HOME) {
    currentPage = MENU_1;
    drawMenu1(selectedOption);
  } else {
    currentPage = HOME;
    drawHomeScreen();
  }
}

Page getCurrentPage() {
  return currentPage;
}
