#ifndef MENU_H
#define MENU_H

enum Page { HOME, MENU_1 };

extern Page currentPage;
extern int selectedOption;
extern const int totalOptions;

void handleMenuInput(int delta);
void handleButtonPress();
Page getCurrentPage();

#endif // MENU_H
