#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "protection.h"
#include "charging.h"
#include "system.h"
#include "telemetry.h"
#include "lcd.h"
#include "io_panel.h"

static bool isI2CDevicePresent(uint8_t address)
{
    Wire.beginTransmission(address);
    return Wire.endTransmission() == 0;
}

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
    pinMode(buttonLeft, INPUT_PULLDOWN);
    pinMode(buttonRight, INPUT_PULLDOWN);
    pinMode(buttonBack, INPUT_PULLDOWN);
    pinMode(buttonSelect, INPUT_PULLDOWN);

    ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
    ledcAttachPin(buck_IN, pwmChannel);
    ledcWrite(pwmChannel, PWM);
    pwmMax = pow(2, pwmResolution) - 1;
    pwmMaxLimited = (PWM_MaxDC * pwmMax) / 100.000;

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY);
    Serial.printf("> I2C initialized (SDA=%d, SCL=%d, FREQ=%d)\n", I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY);

    LCD_Connected = isI2CDevicePresent(0x27) || isI2CDevicePresent(0x3F);
    OLED_Connected = isI2CDevicePresent(0x3C) || isI2CDevicePresent(0x3D);

    if (OLED_Connected)
    {
        enableLCD = 0;
        Serial.println("> OLED detected on I2C (0.96in 128x64)");
        IO_Panel_Init();
    }

    if (LCD_Connected)
    {
        lcd.init();
        lcd.backlight();
        Serial.println("> LCD detected on I2C");
    }
    else
    {
        enableLCD = 0;
        Serial.println("> WARNING: LCD not detected. LCD menu disabled.");
    }

    ADC_SetGain();
    ADS_Connected = ads.begin(0x48) || ads.begin(0x49) || ads.begin(0x4A) || ads.begin(0x4B);
    if (ADS_Connected)
    {
        Serial.println("> ADS1015 detected on I2C");
    }
    else
    {
        Serial.println("> WARNING: ADS1015 not detected. ADC reads disabled.");
    }

    buck_Disable();

    xTaskCreatePinnedToCore(coreTwo, "coreTwo", 10000, NULL, 0, &Core2, 0);

    EEPROM.begin(512);
    Serial.println("> FLASH MEMORY: STORAGE INITIALIZED");
    initializeFlashAutoload();
    applyBatteryPreset(true);
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

    if (OLED_Connected)
    {
        IO_Panel_Update();
    }
    else
    {
        LCD_Menu();
    }
}
