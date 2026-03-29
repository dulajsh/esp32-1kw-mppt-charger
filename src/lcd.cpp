#include "lcd.h"
#include "system.h"

extern int buttonRightCommand;
extern int buttonLeftCommand;
extern int buttonBackCommand;
extern int buttonSelectCommand;

void lcdBacklight_Wake()
{
    if (enableLCD == 0 || LCD_Connected == 0)
    {
        return;
    }
    lcd.setBacklight(HIGH);
    prevLCDBackLMillis = millis();
}

void lcdBacklight()
{
    unsigned long backLightInterval;
    if (backlightSleepMode == 0)
    {
        prevLCDBackLMillis = millis();
    }
    else if (backlightSleepMode == 1)
    {
        backLightInterval = 10000;
    }
    else if (backlightSleepMode == 2)
    {
        backLightInterval = 300000;
    }
    else if (backlightSleepMode == 3)
    {
        backLightInterval = 3600000;
    }
    else if (backlightSleepMode == 4)
    {
        backLightInterval = 21600000;
    }
    else if (backlightSleepMode == 5)
    {
        backLightInterval = 43200000;
    }
    else if (backlightSleepMode == 6)
    {
        backLightInterval = 86400000;
    }
    else if (backlightSleepMode == 7)
    {
        backLightInterval = 259200000;
    }
    else if (backlightSleepMode == 8)
    {
        backLightInterval = 604800000;
    }
    else if (backlightSleepMode == 9)
    {
        backLightInterval = 2419200000;
    }

    if (backlightSleepMode > 0 && settingMode == 0)
    {
        currentLCDBackLMillis = millis();
        if (currentLCDBackLMillis - prevLCDBackLMillis >= backLightInterval)
        {
            prevLCDBackLMillis = currentLCDBackLMillis;
            if (LCD_Connected == 1)
            {
                lcd.setBacklight(LOW);
            }
        }
    }
}

void padding100(int padVar)
{
    if (padVar < 10)
    {
        lcd.print("  ");
    }
    else if (padVar < 100)
    {
        lcd.print(" ");
    }
}

void padding10(int padVar)
{
    if (padVar < 10)
    {
        lcd.print(" ");
    }
}

void displayConfig1()
{
    lcd.setCursor(0, 0);
    lcd.print(powerInput, 0);
    lcd.print("W");
    padding100(powerInput);
    lcd.setCursor(5, 0);
    if (Wh < 10)
    {
        lcd.print(Wh, 3);
        lcd.print("Wh ");
    }
    else if (Wh < 100)
    {
        lcd.print(Wh, 2);
        lcd.print("Wh ");
    }
    else if (Wh < 1000)
    {
        lcd.print(Wh, 1);
        lcd.print("Wh ");
    }
    else if (Wh < 10000)
    {
        lcd.print(kWh, 2);
        lcd.print("kWh ");
    }
    else if (Wh < 100000)
    {
        lcd.print(kWh, 1);
        lcd.print("kWh ");
    }
    else if (Wh < 1000000)
    {
        lcd.print(kWh, 0);
        lcd.print("kWh  ");
    }
    else if (Wh < 10000000)
    {
        lcd.print(MWh, 2);
        lcd.print("MWh ");
    }
    else if (Wh < 100000000)
    {
        lcd.print(MWh, 1);
        lcd.print("MWh ");
    }
    else if (Wh < 1000000000)
    {
        lcd.print(MWh, 0);
        lcd.print("MWh  ");
    }
    lcd.setCursor(13, 0);
    lcd.print(daysRunning, 0);
    lcd.setCursor(0, 1);
    lcd.print(batteryPercent);
    lcd.print("%");
    padding100(batteryPercent);
    if (BNC == 0)
    {
        lcd.setCursor(5, 1);
        lcd.print(voltageOutput, 1);
        lcd.print("V");
        padding10(voltageOutput);
    }
    else
    {
        lcd.setCursor(5, 1);
        lcd.print("NOBAT ");
    }
    lcd.setCursor(11, 1);
    lcd.print(currentOutput, 1);
    lcd.print("A");
    padding10(currentOutput);
}

void displayConfig2()
{
    lcd.setCursor(0, 0);
    lcd.print(powerInput, 0);
    lcd.print("W");
    padding100(powerInput);
    lcd.setCursor(5, 0);
    lcd.print(voltageInput, 1);
    lcd.print("V");
    padding10(voltageInput);
    lcd.setCursor(11, 0);
    lcd.print(currentInput, 1);
    lcd.print("A");
    padding10(currentInput);
    lcd.setCursor(0, 1);
    lcd.print(batteryPercent);
    lcd.print("%");
    padding100(batteryPercent);
    if (BNC == 0)
    {
        lcd.setCursor(5, 1);
        lcd.print(voltageOutput, 1);
        lcd.print("V");
        padding10(voltageOutput);
    }
    else
    {
        lcd.setCursor(5, 1);
        lcd.print("NOBAT");
    }
    lcd.setCursor(11, 1);
    lcd.print(currentOutput, 1);
    lcd.print("A");
    padding10(currentOutput);
}

void displayConfig3()
{
    lcd.setCursor(0, 0);
    lcd.print(powerInput, 0);
    lcd.print("W");
    padding100(powerInput);
    lcd.setCursor(5, 0);
    if (Wh < 10)
    {
        lcd.print(Wh, 2);
        lcd.print("Wh ");
    }
    else if (Wh < 100)
    {
        lcd.print(Wh, 1);
        lcd.print("Wh ");
    }
    else if (Wh < 1000)
    {
        lcd.print(Wh, 0);
        lcd.print("Wh  ");
    }
    else if (Wh < 10000)
    {
        lcd.print(kWh, 1);
        lcd.print("kWh ");
    }
    else if (Wh < 100000)
    {
        lcd.print(kWh, 0);
        lcd.print("kWh  ");
    }
    else if (Wh < 1000000)
    {
        lcd.print(kWh, 0);
        lcd.print("kWh ");
    }
    else if (Wh < 10000000)
    {
        lcd.print(MWh, 1);
        lcd.print("MWh ");
    }
    else if (Wh < 100000000)
    {
        lcd.print(MWh, 0);
        lcd.print("MWh  ");
    }
    else if (Wh < 1000000000)
    {
        lcd.print(MWh, 0);
        lcd.print("MWh ");
    }
    lcd.setCursor(12, 0);
    lcd.print(batteryPercent);
    lcd.print("%");
    padding100(batteryPercent);
    int batteryPercentBars;
    batteryPercentBars = batteryPercent / 6.18;
    lcd.setCursor(0, 1);
    for (int i = 0; i < batteryPercentBars; i++)
    {
        lcd.print((char)255);
    }
    for (int i = 0; i < 16 - batteryPercentBars; i++)
    {
        lcd.print(" ");
    }
}

void displayConfig4()
{
    lcd.setCursor(0, 0);
    lcd.print("TEMPERATURE STAT");
    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.print((char)223);
    lcd.print("C");
    padding100(temperature);
    lcd.setCursor(8, 1);
    lcd.print("FAN");
    lcd.setCursor(12, 1);
    if (fanStatus == 1)
    {
        lcd.print("ON ");
    }
    else
    {
        lcd.print("OFF");
    }
}

void displayConfig5()
{
    lcd.setCursor(0, 0);
    lcd.print(" SETTINGS MENU  ");
    lcd.setCursor(0, 1);
    lcd.print("--PRESS SELECT--");
}

void factoryResetMessageLCD()
{
    lcd.setCursor(0, 0);
    lcd.print("  FACTORY RESET ");
    lcd.setCursor(0, 1);
    lcd.print("   SUCCESSFUL   ");
    delay(1000);
}

void savedMessageLCD()
{
}

void cancelledMessageLCD()
{
}

void LCD_Menu()
{
    int
        menuPages = 4,
        subMenuPages = 14,
        longPressTime = 3000,
        longPressInterval = 500,
        shortPressInterval = 100;
    const int settingsRefreshInterval = 150;

    if (settingMode == 1)
    {
        currentLCDMillis = millis();
        if (currentLCDMillis - prevLCDMillis < settingsRefreshInterval)
        {
            return;
        }
        prevLCDMillis = currentLCDMillis;

        chargingPause = 1;

        if (setMenuPage == 0)
        {
            if (digitalRead(buttonRight) == 1)
            {
                subMenuPage++;
            }
            if (digitalRead(buttonLeft) == 1)
            {
                subMenuPage--;
            }
            if (digitalRead(buttonBack) == 1)
            {
                settingMode = 0;
                subMenuPage = 0;
            }
            if (digitalRead(buttonSelect) == 1)
            {
                setMenuPage = 1;
            }
            lcdBacklight_Wake();
            while (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1 || digitalRead(buttonBack) == 1 || digitalRead(buttonSelect) == 1)
            {
            }
        }

        if (subMenuPage > subMenuPages)
        {
            subMenuPage = 0;
        }
        else if (subMenuPage < 0)
        {
            subMenuPage = subMenuPages;
        }

        if (subMenuPage == 0)
        {
            lcd.setCursor(0, 0);
            lcd.print("SUPPLY ALGORITHM");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            if (MPPT_Mode == 1)
            {
                lcd.print("MPPT + CC-CV  ");
            }
            else
            {
                lcd.print("CC-CV ONLY    ");
            }

            if (setMenuPage == 0)
            {
                boolTemp = MPPT_Mode;
            }
            else
            {
                if (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                    {
                    }
                    if (MPPT_Mode == 1)
                    {
                        MPPT_Mode = 0;
                    }
                    else
                    {
                        MPPT_Mode = 1;
                    }
                }
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    MPPT_Mode = boolTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
            }
        }

        else if (subMenuPage == 1)
        {
            lcd.setCursor(0, 0);
            lcd.print("CHARGER/PSU MODE");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            if (output_Mode == 1)
            {
                lcd.print("CHARGER MODE  ");
            }
            else
            {
                lcd.print("PSU MODE      ");
            }

            if (setMenuPage == 0)
            {
                boolTemp = output_Mode;
            }
            else
            {
                if (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                    {
                    }
                    if (output_Mode == 1)
                    {
                        output_Mode = 0;
                    }
                    else
                    {
                        output_Mode = 1;
                    }
                }
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    output_Mode = boolTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
            }
        }

        else if (subMenuPage == 2)
        {
            lcd.setCursor(0, 0);
            lcd.print("MAX BATTERY V   ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            lcd.print(voltageBatteryMax, 2);
            lcd.print("V");
            lcd.print("                ");

            if (setMenuPage == 0)
            {
                floatTemp = voltageBatteryMax;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    voltageBatteryMax = floatTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                currentMenuSetMillis = millis();
                if (digitalRead(buttonRight) == 1)
                {
                    while (digitalRead(buttonRight) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            voltageBatteryMax += 1.00;
                            voltageBatteryMax = constrain(voltageBatteryMax, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMax, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            voltageBatteryMax += 0.01;
                            voltageBatteryMax = constrain(voltageBatteryMax, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMax, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonLeft) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            voltageBatteryMax -= 1.00;
                            voltageBatteryMax = constrain(voltageBatteryMax, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMax, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            voltageBatteryMax -= 0.01;
                            voltageBatteryMax = constrain(voltageBatteryMax, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMax, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
            }
        }

        else if (subMenuPage == 3)
        {
            lcd.setCursor(0, 0);
            lcd.print("MIN BATTERY V   ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            lcd.print(voltageBatteryMin, 2);
            lcd.print("V");
            lcd.print("                ");

            if (setMenuPage == 0)
            {
                floatTemp = voltageBatteryMin;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    voltageBatteryMin = floatTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                currentMenuSetMillis = millis();
                if (digitalRead(buttonRight) == 1)
                {
                    while (digitalRead(buttonRight) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            voltageBatteryMin += 1.00;
                            voltageBatteryMin = constrain(voltageBatteryMin, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMin, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            voltageBatteryMin += 0.01;
                            voltageBatteryMin = constrain(voltageBatteryMin, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMin, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonLeft) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            voltageBatteryMin -= 1.00;
                            voltageBatteryMin = constrain(voltageBatteryMin, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMin, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            voltageBatteryMin -= 0.01;
                            voltageBatteryMin = constrain(voltageBatteryMin, vOutSystemMin, vOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(voltageBatteryMin, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
            }
        }

        else if (subMenuPage == 4)
        {
            lcd.setCursor(0, 0);
            lcd.print("CHARGING CURRENT");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            lcd.print(currentCharging, 2);
            lcd.print("A");
            lcd.print("                ");

            if (setMenuPage == 0)
            {
                floatTemp = currentCharging;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    currentCharging = floatTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                currentMenuSetMillis = millis();
                if (digitalRead(buttonRight) == 1)
                {
                    while (digitalRead(buttonRight) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            currentCharging += 1.00;
                            currentCharging = constrain(currentCharging, 0.0, cOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(currentCharging, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            currentCharging += 0.01;
                            currentCharging = constrain(currentCharging, 0.0, cOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(currentCharging, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("A   ");
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonLeft) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            currentCharging -= 1.00;
                            currentCharging = constrain(currentCharging, 0.0, cOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(currentCharging, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            currentCharging -= 0.01;
                            currentCharging = constrain(currentCharging, 0.0, cOutSystemMax);
                            lcd.setCursor(2, 1);
                            lcd.print(currentCharging, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("A   ");
                    }
                }
            }
        }

        else if (subMenuPage == 5)
        {
            lcd.setCursor(0, 0);
            lcd.print("COOLING FAN     ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            if (enableFan == 1)
            {
                lcd.print("ENABLED       ");
            }
            else
            {
                lcd.print("DISABLE         ");
            }

            if (setMenuPage == 0)
            {
                boolTemp = enableFan;
            }
            else
            {
                if (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                    {
                    }
                    if (enableFan == 1)
                    {
                        enableFan = 0;
                    }
                    else
                    {
                        enableFan = 1;
                    }
                }
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    enableFan = boolTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
            }
        }

        else if (subMenuPage == 6)
        {
            lcd.setCursor(0, 0);
            lcd.print("FAN TRIGGER TEMP");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            lcd.print(temperatureFan);
            lcd.print((char)223);
            lcd.print("C");
            lcd.print("                ");

            if (setMenuPage == 0)
            {
                intTemp = temperatureFan;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    temperatureFan = intTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                if (digitalRead(buttonRight) == 1)
                {
                    while (digitalRead(buttonRight) == 1)
                    {
                        temperatureFan++;
                        temperatureFan = constrain(temperatureFan, 0, 100);
                        lcd.setCursor(2, 1);
                        lcd.print(temperatureFan);
                        delay(shortPressInterval);
                        lcd.print((char)223);
                        lcd.print("C    ");
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonLeft) == 1)
                    {
                        temperatureFan--;
                        temperatureFan = constrain(temperatureFan, 0, 100);
                        lcd.setCursor(2, 1);
                        lcd.print(temperatureFan);
                        delay(shortPressInterval);
                        lcd.print((char)223);
                        lcd.print("C    ");
                    }
                }
            }
        }

        else if (subMenuPage == 7)
        {
            lcd.setCursor(0, 0);
            lcd.print("SHUTDOWN TEMP   ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            lcd.print(temperatureMax);
            lcd.print((char)223);
            lcd.print("C");
            lcd.print("                ");

            if (setMenuPage == 0)
            {
                intTemp = temperatureMax;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    temperatureMax = intTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                if (digitalRead(buttonRight) == 1)
                {
                    while (digitalRead(buttonRight) == 1)
                    {
                        temperatureMax++;
                        temperatureMax = constrain(temperatureMax, 0, 120);
                        lcd.setCursor(2, 1);
                        lcd.print(temperatureMax);
                        delay(shortPressInterval);
                        lcd.print((char)223);
                        lcd.print("C    ");
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonLeft) == 1)
                    {
                        temperatureMax--;
                        temperatureMax = constrain(temperatureMax, 0, 120);
                        lcd.setCursor(2, 1);
                        lcd.print(temperatureMax);
                        delay(shortPressInterval);
                        lcd.print((char)223);
                        lcd.print("C    ");
                    }
                }
            }
        }

        else if (subMenuPage == 8)
        {
            lcd.setCursor(0, 0);
            lcd.print("WIFI FEATURE    ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            if (enableWiFi == 1)
            {
                lcd.print("ENABLED       ");
            }
            else
            {
                lcd.print("DISABLED      ");
            }

            if (setMenuPage == 0)
            {
                boolTemp = enableWiFi;
            }
            else
            {
                if (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                    {
                    }
                    if (enableWiFi == 1)
                    {
                        enableWiFi = 0;
                    }
                    else
                    {
                        enableWiFi = 1;
                    }
                }
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    enableWiFi = boolTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
            }
        }

        else if (subMenuPage == 9)
        {
            lcd.setCursor(0, 0);
            lcd.print("AUTOLOAD FEATURE");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            if (flashMemLoad == 1)
            {
                lcd.print("ENABLED       ");
            }
            else
            {
                lcd.print("DISABLED      ");
            }

            if (setMenuPage == 0)
            {
                boolTemp = flashMemLoad;
            }
            else
            {
                if (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                    {
                    }
                    if (flashMemLoad == 1)
                    {
                        flashMemLoad = 0;
                    }
                    else
                    {
                        flashMemLoad = 1;
                    }
                }
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    flashMemLoad = boolTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveAutoloadSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
            }
        }

        else if (subMenuPage == 10)
        {
            lcd.setCursor(0, 0);
            lcd.print("BACKLIGHT SLEEP ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            if (backlightSleepMode == 1)
            {
                lcd.print("10 SECONDS    ");
            }
            else if (backlightSleepMode == 2)
            {
                lcd.print("5 MINUTES     ");
            }
            else if (backlightSleepMode == 3)
            {
                lcd.print("1 HOUR        ");
            }
            else if (backlightSleepMode == 4)
            {
                lcd.print("6 HOURS       ");
            }
            else if (backlightSleepMode == 5)
            {
                lcd.print("12 HOURS      ");
            }
            else if (backlightSleepMode == 6)
            {
                lcd.print("1 DAY         ");
            }
            else if (backlightSleepMode == 7)
            {
                lcd.print("3 DAYS        ");
            }
            else if (backlightSleepMode == 8)
            {
                lcd.print("1 WEEK        ");
            }
            else if (backlightSleepMode == 9)
            {
                lcd.print("1 MONTH       ");
            }
            else
            {
                lcd.print("NEVER         ");
            }

            if (setMenuPage == 0)
            {
                intTemp = backlightSleepMode;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    backlightSleepMode = intTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                if (digitalRead(buttonRight) == 1)
                {
                    backlightSleepMode++;
                    backlightSleepMode = constrain(backlightSleepMode, 0, 9);
                    lcd.setCursor(2, 1);
                    if (backlightSleepMode == 1)
                    {
                        lcd.print("10 SECONDS    ");
                    }
                    else if (backlightSleepMode == 2)
                    {
                        lcd.print("5 MINUTES     ");
                    }
                    else if (backlightSleepMode == 3)
                    {
                        lcd.print("1 HOUR        ");
                    }
                    else if (backlightSleepMode == 4)
                    {
                        lcd.print("6 HOURS       ");
                    }
                    else if (backlightSleepMode == 5)
                    {
                        lcd.print("12 HOURS      ");
                    }
                    else if (backlightSleepMode == 6)
                    {
                        lcd.print("1 DAY         ");
                    }
                    else if (backlightSleepMode == 7)
                    {
                        lcd.print("3 DAYS        ");
                    }
                    else if (backlightSleepMode == 8)
                    {
                        lcd.print("1 WEEK        ");
                    }
                    else if (backlightSleepMode == 9)
                    {
                        lcd.print("1 MONTH       ");
                    }
                    else
                    {
                        lcd.print("NEVER         ");
                    }
                    while (digitalRead(buttonRight) == 1)
                    {
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    backlightSleepMode--;
                    backlightSleepMode = constrain(backlightSleepMode, 0, 9);
                    lcd.setCursor(2, 1);
                    if (backlightSleepMode == 1)
                    {
                        lcd.print("10 SECONDS    ");
                    }
                    else if (backlightSleepMode == 2)
                    {
                        lcd.print("5 MINUTES     ");
                    }
                    else if (backlightSleepMode == 3)
                    {
                        lcd.print("1 HOUR        ");
                    }
                    else if (backlightSleepMode == 4)
                    {
                        lcd.print("6 HOURS       ");
                    }
                    else if (backlightSleepMode == 5)
                    {
                        lcd.print("12 HOURS      ");
                    }
                    else if (backlightSleepMode == 6)
                    {
                        lcd.print("1 DAY         ");
                    }
                    else if (backlightSleepMode == 7)
                    {
                        lcd.print("3 DAYS        ");
                    }
                    else if (backlightSleepMode == 8)
                    {
                        lcd.print("1 WEEK        ");
                    }
                    else if (backlightSleepMode == 9)
                    {
                        lcd.print("1 MONTH       ");
                    }
                    else
                    {
                        lcd.print("NEVER         ");
                    }
                    while (digitalRead(buttonLeft) == 1)
                    {
                    }
                }
            }
        }

        else if (subMenuPage == 11)
        {
            lcd.setCursor(0, 0);
            lcd.print("VOUT OFFSET CAL ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            lcd.print(outVoltageOffset, 2);
            lcd.print("V");
            lcd.print("            ");

            if (setMenuPage == 0)
            {
                floatTemp = outVoltageOffset;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    outVoltageOffset = floatTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                currentMenuSetMillis = millis();
                if (digitalRead(buttonRight) == 1)
                {
                    while (digitalRead(buttonRight) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            outVoltageOffset += 0.10;
                            outVoltageOffset = constrain(outVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(outVoltageOffset, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            outVoltageOffset += 0.01;
                            outVoltageOffset = constrain(outVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(outVoltageOffset, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonLeft) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            outVoltageOffset -= 0.10;
                            outVoltageOffset = constrain(outVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(outVoltageOffset, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            outVoltageOffset -= 0.01;
                            outVoltageOffset = constrain(outVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(outVoltageOffset, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
            }
        }

        else if (subMenuPage == 12)
        {
            lcd.setCursor(0, 0);
            lcd.print("VIN OFFSET CAL  ");
            if (setMenuPage == 1)
            {
                lcd.setCursor(0, 1);
                lcd.print(" >");
            }
            else
            {
                lcd.setCursor(0, 1);
                lcd.print("= ");
            }
            lcd.setCursor(2, 1);
            lcd.print(inVoltageOffset, 2);
            lcd.print("V");
            lcd.print("            ");

            if (setMenuPage == 0)
            {
                floatTemp = inVoltageOffset;
            }
            else
            {
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    inVoltageOffset = floatTemp;
                    cancelledMessageLCD();
                    setMenuPage = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    saveSettings();
                    setMenuPage = 0;
                    savedMessageLCD();
                }
                currentMenuSetMillis = millis();
                if (digitalRead(buttonRight) == 1)
                {
                    while (digitalRead(buttonRight) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            inVoltageOffset += 0.10;
                            inVoltageOffset = constrain(inVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(inVoltageOffset, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            inVoltageOffset += 0.01;
                            inVoltageOffset = constrain(inVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(inVoltageOffset, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
                else if (digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonLeft) == 1)
                    {
                        if (millis() - currentMenuSetMillis > longPressTime)
                        {
                            inVoltageOffset -= 0.10;
                            inVoltageOffset = constrain(inVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(inVoltageOffset, 2);
                            delay(longPressInterval);
                        }
                        else
                        {
                            inVoltageOffset -= 0.01;
                            inVoltageOffset = constrain(inVoltageOffset, -5.0, 5.0);
                            lcd.setCursor(2, 1);
                            lcd.print(inVoltageOffset, 2);
                            delay(shortPressInterval);
                        }
                        lcd.print("V   ");
                    }
                }
            }
        }

        else if (subMenuPage == 13)
        {
            if (setMenuPage == 0)
            {
                lcd.setCursor(0, 0);
                lcd.print("FACTORY RESET   ");
                lcd.setCursor(0, 1);
                lcd.print("> PRESS SELECT  ");
            }
            else
            {
                if (confirmationMenu == 0)
                {
                    lcd.setCursor(0, 0);
                    lcd.print(" ARE YOU SURE?  ");
                    lcd.setCursor(0, 1);
                    lcd.print("  >NO      YES  ");
                }
                else
                {
                    lcd.setCursor(0, 0);
                    lcd.print(" ARE YOU SURE?  ");
                    lcd.setCursor(0, 1);
                    lcd.print("   NO     >YES  ");
                }
                if (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                {
                    while (digitalRead(buttonRight) == 1 || digitalRead(buttonLeft) == 1)
                    {
                    }
                    if (confirmationMenu == 0)
                    {
                        confirmationMenu = 1;
                    }
                    else
                    {
                        confirmationMenu = 0;
                    }
                }
                if (digitalRead(buttonBack) == 1)
                {
                    while (digitalRead(buttonBack) == 1)
                    {
                    }
                    cancelledMessageLCD();
                    setMenuPage = 0;
                    confirmationMenu = 0;
                }
                if (digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonSelect) == 1)
                    {
                    }
                    if (confirmationMenu == 1)
                    {
                        factoryReset();
                        factoryResetMessageLCD();
                    }
                    setMenuPage = 0;
                    confirmationMenu = 0;
                    subMenuPage = 0;
                }
            }
        }

        else if (subMenuPage == 14)
        {
            if (setMenuPage == 0)
            {
                lcd.setCursor(0, 0);
                lcd.print("FIRMWARE VERSION");
                lcd.setCursor(0, 1);
                lcd.print(firmwareInfo);
                lcd.setCursor(8, 1);
                lcd.print(firmwareDate);
            }
            else
            {
                lcd.setCursor(0, 0);
                lcd.print(firmwareContactR1);
                lcd.setCursor(0, 1);
                lcd.print(firmwareContactR2);
                if (digitalRead(buttonBack) == 1 || digitalRead(buttonSelect) == 1)
                {
                    while (digitalRead(buttonBack) == 1 || digitalRead(buttonSelect) == 1)
                    {
                    }
                    setMenuPage = 0;
                }
            }
        }
    }

    else if (settingMode == 0)
    {
        chargingPause = 0;

        lcdBacklight();

        if (digitalRead(buttonRight) == 1)
        {
            buttonRightCommand = 1;
            lcdBacklight_Wake();
        }
        if (digitalRead(buttonLeft) == 1)
        {
            buttonLeftCommand = 1;
            lcdBacklight_Wake();
        }
        if (digitalRead(buttonBack) == 1)
        {
            buttonBackCommand = 1;
            lcdBacklight_Wake();
        }
        if (digitalRead(buttonSelect) == 1)
        {
            buttonSelectCommand = 1;
            lcdBacklight_Wake();
        }

        currentLCDMillis = millis();
        if (currentLCDMillis - prevLCDMillis >= millisLCDInterval && enableLCD == 1)
        {
            prevLCDMillis = currentLCDMillis;

            if (buttonRightCommand == 1)
            {
                buttonRightCommand = 0;
                menuPage++;
                lcd.clear();
            }
            else if (buttonLeftCommand == 1)
            {
                buttonLeftCommand = 0;
                menuPage--;
                lcd.clear();
            }
            else if (buttonBackCommand == 1)
            {
                buttonBackCommand = 0;
                menuPage = 0;
                lcd.clear();
            }
            else if (buttonSelectCommand == 1 && menuPage == 4)
            {
                buttonSelectCommand = 0;
                settingMode = 1;
                lcd.clear();
            }
            if (menuPage > menuPages)
            {
                menuPage = 0;
            }
            else if (menuPage < 0)
            {
                menuPage = menuPages;
            }

            if (menuPage == 0)
            {
                displayConfig1();
            }
            else if (menuPage == 1)
            {
                displayConfig2();
            }
            else if (menuPage == 2)
            {
                displayConfig3();
            }
            else if (menuPage == 3)
            {
                displayConfig4();
            }
            else if (menuPage == 4)
            {
                displayConfig5();
            }
        }
    }
}
