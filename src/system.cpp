#include "system.h"
#include "sensors.h"

void System_Processes()
{
    if (enableFan == true)
    {
        if (enableDynamicCooling == false)
        {
            if (overrideFan == true)
            {
                fanStatus = true;
            }
            else if (temperature >= temperatureFan)
            {
                fanStatus = 1;
            }
            else if (temperature < temperatureFan)
            {
                fanStatus = 0;
            }
            digitalWrite(FAN, fanStatus);
        }
        else
        {
        }
    }
    else
    {
        digitalWrite(FAN, LOW);
    }

    loopTimeStart = micros();
    loopTime = (loopTimeStart - loopTimeEnd) / 1000.000;
    loopTimeEnd = micros();

    if (telemCounterReset == 0)
    {
    }
    else if (telemCounterReset == 1 && daysRunning > 1)
    {
        resetVariables();
    }
    else if (telemCounterReset == 2 && daysRunning > 7)
    {
        resetVariables();
    }
    else if (telemCounterReset == 3 && daysRunning > 30)
    {
        resetVariables();
    }
    else if (telemCounterReset == 4 && daysRunning > 365)
    {
        resetVariables();
    }

    if (lowPowerMode == 1)
    {
    }
    else
    {
    }
}

void factoryReset()
{
    EEPROM.write(0, 1);
    EEPROM.write(12, 1);
    EEPROM.write(1, 12);
    EEPROM.write(2, 0);
    EEPROM.write(3, 9);
    EEPROM.write(4, 0);
    EEPROM.write(5, 30);
    EEPROM.write(6, 0);
    EEPROM.write(7, 1);
    EEPROM.write(8, 60);
    EEPROM.write(9, 90);
    EEPROM.write(10, 1);
    EEPROM.write(11, 1);
    EEPROM.write(13, 0);
    EEPROM.commit();
    loadSettings();
}

void loadSettings()
{
    MPPT_Mode = EEPROM.read(0);
    output_Mode = EEPROM.read(12);
    voltageBatteryMax = EEPROM.read(1) + (EEPROM.read(2) * .01);
    voltageBatteryMin = EEPROM.read(3) + (EEPROM.read(4) * .01);
    currentCharging = EEPROM.read(5) + (EEPROM.read(6) * .01);
    enableFan = EEPROM.read(7);
    temperatureFan = EEPROM.read(8);
    temperatureMax = EEPROM.read(9);
    enableWiFi = EEPROM.read(10);
    flashMemLoad = EEPROM.read(11);
    backlightSleepMode = EEPROM.read(13);
}

void saveSettings()
{
    EEPROM.write(0, MPPT_Mode);
    EEPROM.write(12, output_Mode);
    conv1 = voltageBatteryMax * 100;
    conv2 = conv1 % 100;
    EEPROM.write(1, voltageBatteryMax);
    EEPROM.write(2, conv2);
    conv1 = voltageBatteryMin * 100;
    conv2 = conv1 % 100;
    EEPROM.write(3, voltageBatteryMin);
    EEPROM.write(4, conv2);
    conv1 = currentCharging * 100;
    conv2 = conv1 % 100;
    EEPROM.write(5, currentCharging);
    EEPROM.write(6, conv2);
    EEPROM.write(7, enableFan);
    EEPROM.write(8, temperatureFan);
    EEPROM.write(9, temperatureMax);
    EEPROM.write(10, enableWiFi);
    EEPROM.write(13, backlightSleepMode);
    EEPROM.commit();
}

void saveAutoloadSettings()
{
    EEPROM.write(11, flashMemLoad);
    EEPROM.commit();
}

void initializeFlashAutoload()
{
    if (disableFlashAutoLoad == 0)
    {
        flashMemLoad = EEPROM.read(11);
        if (flashMemLoad == 1)
        {
            loadSettings();
        }
    }
}
