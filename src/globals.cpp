#include "config.h"

//===== OBJECT INSTANCES =====
LiquidCrystal_I2C lcd(0x27, 16, 2);
TaskHandle_t Core2;
Adafruit_ADS1015 ads;

//===== WiFi CREDENTIALS =====
char auth[] = "InputBlynkAuthenticationToken";
char ssid[] = "InputWiFiSSID";
char pass[] = "InputWiFiPassword";

//===== USER PARAMETERS =====
bool MPPT_Mode = 1;
bool output_Mode = 1;
bool disableFlashAutoLoad = 0;
bool enablePPWM = 1;
bool enableWiFi = 1;
bool enableFan = 1;
bool enableBluetooth = 1;
bool enableLCD = 1;
bool enableLCDBacklight = 1;
bool overrideFan = 0;
bool enableDynamicCooling = 0;

int serialTelemMode = 1;
int pwmResolution = 11;
int pwmFrequency = 39000;
int temperatureFan = 60;
int temperatureMax = 90;
int telemCounterReset = 0;
int errorTimeLimit = 1000;
int errorCountLimit = 5;
int millisRoutineInterval = 250;
int millisSerialInterval = 1;
int millisLCDInterval = 2000;
int millisWiFiInterval = 2000;
int millisLCDBackLInterval = 2000;
int backlightSleepMode = 0;
int baudRate = 500000;

float voltageBatteryMax = 27.3000;
float voltageBatteryMin = 22.4000;
float currentCharging = 30.0000;
float electricalPrice = 9.5000;
int batteryPreset = 0;

//===== CALIBRATION PARAMETERS =====
bool ADS1015_Mode = 1;
int ADC_GainSelect = 2;
int avgCountVS = 3;
int avgCountCS = 4;
int avgCountTS = 500;

float inVoltageDivRatio = 40.2156;
float inVoltageOffset = 0.000;
float outVoltageDivRatio = 24.5000;
float outVoltageOffset = 0.000;
float vOutSystemMax = 50.0000;
float cOutSystemMax = 50.0000;
float ntcResistance = 10000.00;
float voltageDropout = 1.0000;
float voltageBatteryThresh = 1.5000;
float currentInAbsolute = 31.0000;
float currentOutAbsolute = 50.0000;
float PPWM_margin = 99.5000;
float PWM_MaxDC = 97.0000;
float efficiencyRate = 1.0000;
float currentMidPoint = 2.5250;
float currentSens = 0.0000;
float currentSensV = 0.0660;
float vInSystemMin = 10.000;

//===== SYSTEM PARAMETERS (BOOLEANS) =====
bool buckEnable = 0;
bool fanStatus = 0;
bool bypassEnable = 0;
bool chargingPause = 0;
bool lowPowerMode = 0;
bool settingMode = 0;
bool boolTemp = 0;
bool flashMemLoad = 1;
bool confirmationMenu = 0;
bool WIFI = 0;
bool BNC = 0;
bool REC = 0;
bool FLV = 0;
bool IUV = 0;
bool IOC = 0;
bool OOV = 0;
bool OOC = 0;
bool OTE = 0;
bool ADS_Connected = 0;
bool LCD_Connected = 0;
bool OLED_Connected = 0;

//===== SYSTEM PARAMETERS (INTEGERS) =====
int inputSource = 0;
int temperature = 0;
int sampleStoreTS = 0;
int pwmMax = 0;
int pwmMaxLimited = 0;
int PWM = 0;
int PPWM = 0;
int pwmChannel = 0;
int batteryPercent = 0;
int errorCount = 0;
int menuPage = 0;
int subMenuPage = 0;
int ERR = 0;
int conv1 = 0;
int conv2 = 0;
int intTemp = 0;
int setMenuPage = 0;
int buttonRightCommand = 0;
int buttonLeftCommand = 0;
int buttonBackCommand = 0;
int buttonSelectCommand = 0;

//===== SYSTEM PARAMETERS (FLOATS) =====
float VSI = 0.0000;
float VSO = 0.0000;
float CSI = 0.0000;
float CSI_converted = 0.0000;
float TS = 0.0000;
float powerInput = 0.0000;
float powerInputPrev = 0.0000;
float powerOutput = 0.0000;
float energySavings = 0.0000;
float voltageInput = 0.0000;
float voltageInputPrev = 0.0000;
float voltageOutput = 0.0000;
float currentInput = 0.0000;
float currentOutput = 0.0000;
float TSlog = 0.0000;
float ADC_BitReso = 0.0000;
float daysRunning = 0.0000;
float Wh = 0.0000;
float kWh = 0.0000;
float MWh = 0.0000;
float loopTime = 0.0000;
float outputDeviation = 0.0000;
float buckEfficiency = 0.0000;
float floatTemp = 0.0000;
float vOutSystemMin = 0.0000;
float chargeVoltageTarget = 27.3000;
float chargeCurrentTarget = 30.0000;
float batteryAbsorptionVoltage = 27.3000;
float batteryFloatVoltage = 26.4000;
float batteryRechargeVoltage = 25.2000;
float batteryAbsExitCurrent = 3.0000;

//===== SYSTEM PARAMETERS (TIMING) =====
unsigned long currentErrorMillis = 0;
unsigned long currentSerialMillis = 0;
unsigned long currentRoutineMillis = 0;
unsigned long currentLCDMillis = 0;
unsigned long currentLCDBackLMillis = 0;
unsigned long currentWiFiMillis = 0;
unsigned long currentMenuSetMillis = 0;
unsigned long prevSerialMillis = 0;
unsigned long prevRoutineMillis = 0;
unsigned long prevErrorMillis = 0;
unsigned long prevLCDMillis = 0;
unsigned long prevLCDBackLMillis = 0;
unsigned long timeOn = 0;
unsigned long loopTimeStart = 0;
unsigned long loopTimeEnd = 0;
unsigned long secondsElapsed = 0;
unsigned long chargingStageStartMillis = 0;

//===== CHARGING STAGE PARAMETERS =====
int chargingStage = 0;
bool batteryFloatEnabled = 1;
unsigned long batteryAbsorptionTimeoutMs = 7200000;

//===== FIRMWARE INFO =====
String firmwareInfo = "V1.10   ";
String firmwareDate = "30/08/21";
String firmwareContactR1 = "www.youtube.com/";
String firmwareContactR2 = "TechBuilder     ";
