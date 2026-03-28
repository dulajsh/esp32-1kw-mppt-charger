#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "protection.h"
#include "charging.h"
#include "system.h"
#include "telemetry.h"
#include "lcd.h"

//===== CORE0: SETUP & LOOP (DUAL CORE MODE) =====
void coreTwo(void *pvParameters)
{
    setupWiFi();
    while (1)
    {
        Wireless_Telemetry();
    }
}

//===== CORE1: SETUP (DUAL CORE MODE) =====
void setup()
{
    Serial.begin(baudRate);
    Serial.println("> Serial Initialized");

    pinMode(backflow_MOSFET, OUTPUT);
    pinMode(buck_EN, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(FAN, OUTPUT);
    pinMode(TempSensor, INPUT);
    pinMode(ADC_ALERT, INPUT);
    pinMode(buttonLeft, INPUT);
    pinMode(buttonRight, INPUT);
    pinMode(buttonBack, INPUT);
    pinMode(buttonSelect, INPUT);

    ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
    ledcAttachPin(buck_IN, pwmChannel);
    ledcWrite(pwmChannel, PWM);
    pwmMax = pow(2, pwmResolution) - 1;
    pwmMaxLimited = (PWM_MaxDC * pwmMax) / 100.000;

    ADC_SetGain();
    ads.begin();

    buck_Disable();

    xTaskCreatePinnedToCore(coreTwo, "coreTwo", 10000, NULL, 0, &Core2, 0);

    EEPROM.begin(512);
    Serial.println("> FLASH MEMORY: STORAGE INITIALIZED");
    initializeFlashAutoload();
    Serial.println("> FLASH MEMORY: SAVED DATA LOADED");

    if (enableLCD == 1)
    {
        lcd.setBacklight(HIGH);
        lcd.setCursor(0, 0);
        lcd.print("MPPT INITIALIZED");
        lcd.setCursor(0, 1);
        lcd.print("FIRMWARE ");
        lcd.print(firmwareInfo);
        delay(1500);
        lcd.clear();
    }

    Serial.println("> MPPT HAS INITIALIZED");
}

//===== CORE1: LOOP (DUAL CORE MODE) =====
void loop()
{
    Read_Sensors();
    Device_Protection();
    System_Processes();
    Charging_Algorithm();
    Onboard_Telemetry();
    LCD_Menu();
}
