#ifndef LCD_H
#define LCD_H

#include "config.h"

void lcdBacklight_Wake();
void lcdBacklight();
void padding100(int padVar);
void padding10(int padVar);
void displayConfig1();
void displayConfig2();
void displayConfig3();
void displayConfig4();
void displayConfig5();
void factoryResetMessageLCD();
void savedMessageLCD();
void cancelledMessageLCD();
void LCD_Menu();

#endif
