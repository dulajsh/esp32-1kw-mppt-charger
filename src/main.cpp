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

//===== CORE0: HIGH-PRIORITY UI & ENCODER TASK =====
void uiTask(void *pvParameters)
{
    while (1)
    {
        if (otaUpdating)
        {
            if (OLED_Connected)
            {
                IO_Panel_ShowOTAProgress(otaProgressPercent, otaState);
            }
            else if (LCD_Connected)
            {
                static unsigned int lastLcdPercent = 999;
                if (otaProgressPercent != lastLcdPercent)
                {
                    lastLcdPercent = otaProgressPercent;
                    if (lockI2C(pdMS_TO_TICKS(25)))
                    {
                        lcd.setCursor(0, 0);
                        lcd.print("OTA UPDATING... ");
                        lcd.setCursor(0, 1);
                        lcd.printf("Progress: %3u%%  ", otaProgressPercent);
                        unlockI2C();
                    }
                }
            }
            vTaskDelay(pdMS_TO_TICKS(30));
            continue;
        }

        if (OLED_Connected)
        {
            IO_Panel_Update();
        }
        else if (LCD_Connected)
        {
            LCD_Menu();
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

//===== CORE0: BACKGROUND NETWORK & TELEMETRY TASK =====
void coreTwo(void *pvParameters)
{
    setupWiFi();
    unsigned long lastSystemMillis = 0;

    while (1)
    {
        // 1. WiFi & network telemetry
        Wireless_Telemetry();

        unsigned long now = millis();

        // 2. System housekeeping & serial telemetry (~100ms)
        if (now - lastSystemMillis >= 100)
        {
            lastSystemMillis = now;
            System_Processes();
            Onboard_Telemetry();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
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

    i2cMutex = xSemaphoreCreateMutex();
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

    uint8_t adsAddress = 0;
    if (ads.begin(0x48)) adsAddress = 0x48;
    else if (ads.begin(0x49)) adsAddress = 0x49;
    else if (ads.begin(0x4A)) adsAddress = 0x4A;
    else if (ads.begin(0x4B)) adsAddress = 0x4B;

    ADS_Connected = (adsAddress != 0);
    adsDetectedAddress = adsAddress;
    if (ADS_Connected)
    {
        ADC_SetGain();
        Serial.printf("> ADS1115 detected on I2C address 0x%02X\n", adsAddress);
    }
    else
    {
        Serial.println("> WARNING: ADS1115 not detected on I2C (0x48-0x4B). ADC reads disabled.");
    }

    buck_Disable();

    xTaskCreatePinnedToCore(uiTask, "uiTask", 4096, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(coreTwo, "coreTwo", 10000, NULL, 1, &Core2, 0);

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

//===== CORE1: DEDICATED HIGH-SPEED POWER CONVERSION LOOP =====
void loop()
{
    if (otaUpdating)
    {
        buck_Disable();
        ledcWrite(pwmChannel, 0);
        bypassEnable = 0;
        digitalWrite(backflow_MOSFET, LOW);
        digitalWrite(FAN, LOW);
        vTaskDelay(pdMS_TO_TICKS(20));
        return;
    }

    loopTimeStart = micros();
    if (loopTimeEnd > 0)
    {
        loopTime = (loopTimeStart - loopTimeEnd) / 1000.0f;
    }
    loopTimeEnd = loopTimeStart;

    Read_Sensors();
    Device_Protection();
    Charging_Algorithm();

    vTaskDelay(pdMS_TO_TICKS(4));
}
