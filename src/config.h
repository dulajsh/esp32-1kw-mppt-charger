#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_ADS1X15.h>

//===== OBJECT INSTANCES =====
extern LiquidCrystal_I2C lcd;
extern TaskHandle_t Core2;
extern Adafruit_ADS1015 ads;

//===== PIN DEFINITIONS =====
#define backflow_MOSFET 27
#define buck_IN 33
#define buck_EN 32
#define LED 2
#define FAN 16
#define ADC_ALERT 34
#define TempSensor 35
#define buttonLeft 18
#define buttonRight 17
#define buttonBack 19
#define buttonSelect 23
#define encoderPinA 17
#define encoderPinB 18
#define encoderPinSW 19
// 1 = encoder common wired to +3.3V (use pulldown, active HIGH)
// 0 = encoder common wired to GND (use pullup, active LOW)
#define encoderCommonPositive 1

//===== I2C BUS DEFINITIONS =====
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQUENCY 100000

//===== WiFi CREDENTIALS =====
extern char auth[];
extern char ssid[];
extern char pass[];

//===== USER PARAMETERS =====
extern bool MPPT_Mode;
extern bool output_Mode;
extern bool disableFlashAutoLoad;
extern bool enablePPWM;
extern bool enableWiFi;
extern bool enableFan;
extern bool enableBluetooth;
extern bool enableLCD;
extern bool enableLCDBacklight;
extern bool overrideFan;
extern bool enableDynamicCooling;

extern int serialTelemMode;
extern int pwmResolution;
extern int pwmFrequency;
extern int temperatureFan;
extern int temperatureMax;
extern int telemCounterReset;
extern int errorTimeLimit;
extern int errorCountLimit;
extern int millisRoutineInterval;
extern int millisSerialInterval;
extern int millisLCDInterval;
extern int millisWiFiInterval;
extern int millisLCDBackLInterval;
extern int backlightSleepMode;
extern int baudRate;

extern float voltageBatteryMax;
extern float voltageBatteryMin;
extern float currentCharging;
extern float electricalPrice;
extern int batteryPreset;

//===== CALIBRATION PARAMETERS =====
extern bool ADS1015_Mode;
extern int ADC_GainSelect;
extern int avgCountVS;
extern int avgCountCS;
extern int avgCountTS;
extern float inVoltageDivRatio;
extern float inVoltageOffset;
extern float outVoltageDivRatio;
extern float outVoltageOffset;
extern float vOutSystemMax;
extern float cOutSystemMax;
extern float ntcResistance;
extern float voltageDropout;
extern float voltageBatteryThresh;
extern float currentInAbsolute;
extern float currentOutAbsolute;
extern float PPWM_margin;
extern float PWM_MaxDC;
extern float efficiencyRate;
extern float currentMidPoint;
extern float currentSens;
extern float currentSensV;
extern float vInSystemMin;

//===== SYSTEM PARAMETERS (BOOLEANS) =====
extern bool buckEnable;
extern bool fanStatus;
extern bool bypassEnable;
extern bool chargingPause;
extern bool lowPowerMode;
extern bool settingMode;
extern bool boolTemp;
extern bool flashMemLoad;
extern bool confirmationMenu;
extern bool WIFI;
extern bool BNC;
extern bool REC;
extern bool FLV;
extern bool IUV;
extern bool IOC;
extern bool OOV;
extern bool OOC;
extern bool OTE;
extern bool ADS_Connected;
extern bool LCD_Connected;
extern bool OLED_Connected;

//===== SYSTEM PARAMETERS (INTEGERS) =====
extern int inputSource;
extern int temperature;
extern int sampleStoreTS;
extern int pwmMax;
extern int pwmMaxLimited;
extern int PWM;
extern int PPWM;
extern int pwmChannel;
extern int batteryPercent;
extern int errorCount;
extern int menuPage;
extern int subMenuPage;
extern int ERR;
extern int conv1;
extern int conv2;
extern int intTemp;
extern int setMenuPage;
extern int buttonRightCommand;
extern int buttonLeftCommand;
extern int buttonBackCommand;
extern int buttonSelectCommand;

//===== SYSTEM PARAMETERS (FLOATS) =====
extern float VSI;
extern float VSO;
extern float CSI;
extern float CSI_converted;
extern float TS;
extern float powerInput;
extern float powerInputPrev;
extern float powerOutput;
extern float energySavings;
extern float voltageInput;
extern float voltageInputPrev;
extern float voltageOutput;
extern float currentInput;
extern float currentOutput;
extern float TSlog;
extern float ADC_BitReso;
extern float daysRunning;
extern float Wh;
extern float kWh;
extern float MWh;
extern float loopTime;
extern float outputDeviation;
extern float buckEfficiency;
extern float floatTemp;
extern float vOutSystemMin;
extern float chargeVoltageTarget;
extern float chargeCurrentTarget;
extern float batteryAbsorptionVoltage;
extern float batteryFloatVoltage;
extern float batteryRechargeVoltage;
extern float batteryAbsExitCurrent;

//===== SYSTEM PARAMETERS (TIMING) =====
extern unsigned long currentErrorMillis;
extern unsigned long currentSerialMillis;
extern unsigned long currentRoutineMillis;
extern unsigned long currentLCDMillis;
extern unsigned long currentLCDBackLMillis;
extern unsigned long currentWiFiMillis;
extern unsigned long currentMenuSetMillis;
extern unsigned long prevSerialMillis;
extern unsigned long prevRoutineMillis;
extern unsigned long prevErrorMillis;
extern unsigned long prevLCDMillis;
extern unsigned long prevLCDBackLMillis;
extern unsigned long timeOn;
extern unsigned long loopTimeStart;
extern unsigned long loopTimeEnd;
extern unsigned long secondsElapsed;
extern unsigned long chargingStageStartMillis;

//===== CHARGING STAGE PARAMETERS =====
extern int chargingStage;
extern bool batteryFloatEnabled;
extern unsigned long batteryAbsorptionTimeoutMs;

//===== FIRMWARE INFO =====
extern String firmwareInfo;
extern String firmwareDate;
extern String firmwareContactR1;
extern String firmwareContactR2;

#endif
