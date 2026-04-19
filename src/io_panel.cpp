#include "io_panel.h"

#include <U8g2lib.h>

#include "charging.h"
#include "system.h"

namespace
{
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

#if encoderCommonPositive
    const uint8_t encoderInputMode = INPUT_PULLDOWN;
    const uint8_t encoderActiveLevel = HIGH;
#else
    const uint8_t encoderInputMode = INPUT_PULLUP;
    const uint8_t encoderActiveLevel = LOW;
#endif

    struct ButtonState
    {
        bool stablePressed = false;
        bool rawPressed = false;
        bool longPressFired = false;
        unsigned long lastChangeMs = 0;
        unsigned long pressedAtMs = 0;
    };

    enum ValueType
    {
        VALUE_BOOL,
        VALUE_INT,
        VALUE_FLOAT
    };

    struct MenuItem
    {
        const char *label;
        ValueType type;
        void *ptr;
        float minVal;
        float maxVal;
        float step;
        uint8_t decimals;
    };

    bool oledSleepEnabled = true;

    MenuItem menuItems[] = {
        {"MPPT Algo", VALUE_BOOL, &MPPT_Mode, 0, 1, 1, 0},
        {"Output Mode", VALUE_BOOL, &output_Mode, 0, 1, 1, 0},
        {"Batt Max V", VALUE_FLOAT, &voltageBatteryMax, 10.0f, 60.0f, 0.01f, 2},
        {"Batt Min V", VALUE_FLOAT, &voltageBatteryMin, 10.0f, 60.0f, 0.01f, 2},
        {"Charge A", VALUE_FLOAT, &currentCharging, 0.0f, 50.0f, 0.01f, 2},
        {"Fan Enable", VALUE_BOOL, &enableFan, 0, 1, 1, 0},
        {"Fan Start C", VALUE_INT, &temperatureFan, 20, 100, 1, 0},
        {"Temp Max C", VALUE_INT, &temperatureMax, 40, 120, 1, 0},
        {"WiFi Enable", VALUE_BOOL, &enableWiFi, 0, 1, 1, 0},
        {"OLED Sleep", VALUE_BOOL, &oledSleepEnabled, 0, 1, 1, 0},
        {"Battery Preset", VALUE_INT, &batteryPreset, 0, 5, 1, 0},
        {"OLED View", VALUE_INT, &oledDisplayMode, 0, 1, 1, 0},
    };

    const int menuItemCount = sizeof(menuItems) / sizeof(menuItems[0]);

    ButtonState encoderSw;

    bool settingsOpen = false;
    bool editValue = false;
    bool refreshRequested = true;
    int selectedIndex = 0;
    int topIndex = 0;
    int statusPage = 0;
    int16_t encoderDelta = 0;
    volatile int8_t encoderDetentsPending = 0;
    volatile int8_t encoderQuarterSteps = 0;
    volatile uint8_t encoderPrevAB = 0;
    portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;
    float editSnapshot = 0.0f;

    const unsigned long statusRedrawIntervalMs = 400;
    const unsigned long settingsRedrawIntervalMs = 120;
    const unsigned long debounceMs = 25;
    const unsigned long longPressMs = 1200;
    const int8_t encoderQuarterStepsPerMove = 2;
    const unsigned long oledSleepTimeoutMs = 20000;
    bool oledSleeping = false;
    unsigned long lastUserActivityMs = 0;
    unsigned long lastEditTurnMs = 0;
    unsigned long lastDrawMs = 0;

    void setOledSleepState(bool sleep)
    {
        if (oledSleeping == sleep)
        {
            return;
        }

        oledSleeping = sleep;
        oled.setPowerSave(oledSleeping ? 1 : 0);
        if (!oledSleeping)
        {
            refreshRequested = true;
        }
    }

    inline uint8_t readEncoderAB()
    {
        const uint8_t a = (digitalRead(encoderPinA) == encoderActiveLevel) ? 1 : 0;
        const uint8_t b = (digitalRead(encoderPinB) == encoderActiveLevel) ? 1 : 0;
        return (a << 1) | b;
    }

    void IRAM_ATTR onEncoderEdge()
    {
        static const int8_t transitionTable[16] = {
            0, -1, 1, 0,
            1, 0, 0, -1,
            -1, 0, 0, 1,
            0, 1, -1, 0};

        const uint8_t ab = readEncoderAB();
        const uint8_t transition = (encoderPrevAB << 2) | ab;
        const int8_t quarterStep = transitionTable[transition];
        encoderPrevAB = ab;

        if (quarterStep == 0)
        {
            return;
        }

        encoderQuarterSteps += quarterStep;

        if (encoderQuarterSteps >= encoderQuarterStepsPerMove)
        {
            if (encoderDetentsPending < 16)
            {
                encoderDetentsPending++;
            }
            encoderQuarterSteps = 0;
        }
        else if (encoderQuarterSteps <= -encoderQuarterStepsPerMove)
        {
            if (encoderDetentsPending > -16)
            {
                encoderDetentsPending--;
            }
            encoderQuarterSteps = 0;
        }
    }

    void updateButton(ButtonState &state, int pin, bool &shortPress, bool &longPress)
    {
        const unsigned long now = millis();
        const bool rawPressed = (digitalRead(pin) == encoderActiveLevel);

        if (rawPressed != state.rawPressed)
        {
            state.rawPressed = rawPressed;
            state.lastChangeMs = now;
        }

        if (now - state.lastChangeMs < debounceMs)
        {
            return;
        }

        if (state.stablePressed != state.rawPressed)
        {
            state.stablePressed = state.rawPressed;
            if (state.stablePressed)
            {
                state.pressedAtMs = now;
                state.longPressFired = false;
            }
            else if (!state.longPressFired)
            {
                shortPress = true;
            }
        }

        if (state.stablePressed && !state.longPressFired && (now - state.pressedAtMs >= longPressMs))
        {
            state.longPressFired = true;
            longPress = true;
        }
    }

    void updateEncoder()
    {
        int8_t detents = 0;
        portENTER_CRITICAL(&encoderMux);
        detents = encoderDetentsPending;
        encoderDetentsPending = 0;
        portEXIT_CRITICAL(&encoderMux);

        if (detents != 0)
        {
            encoderDelta += detents;
        }
    }

    void clampAndApply(MenuItem &item)
    {
        if (item.type == VALUE_BOOL)
        {
            bool &v = *static_cast<bool *>(item.ptr);
            v = (v ? 1 : 0);
            return;
        }

        if (item.type == VALUE_INT)
        {
            int &v = *static_cast<int *>(item.ptr);
            v = constrain(v, (int)item.minVal, (int)item.maxVal);
            return;
        }

        float &v = *static_cast<float *>(item.ptr);
        v = constrain(v, item.minVal, item.maxVal);
    }

    int16_t getAdaptiveEditDelta(const MenuItem &item, int16_t rawDelta)
    {
        if (rawDelta == 0)
        {
            return 0;
        }

        if (item.type == VALUE_BOOL)
        {
            return (rawDelta > 0) ? 1 : -1;
        }

        const unsigned long now = millis();
        const unsigned long dt = (lastEditTurnMs == 0) ? 1000 : (now - lastEditTurnMs);
        lastEditTurnMs = now;

        int gain = 1;
        if (item.type == VALUE_FLOAT)
        {
            // Aggressive turbo profile for fast rotary turns on 0.01-step items.
            if (dt <= 25)
            {
                gain = 400;
            }
            else if (dt <= 40)
            {
                gain = 200;
            }
            else if (dt <= 60)
            {
                gain = 100;
            }
            else if (dt <= 90)
            {
                gain = 50;
            }
            else if (dt <= 130)
            {
                gain = 20;
            }
            else if (dt <= 200)
            {
                gain = 8;
            }
        }
        else
        {
            if (dt <= 30)
            {
                gain = 20;
            }
            else if (dt <= 50)
            {
                gain = 10;
            }
            else if (dt <= 80)
            {
                gain = 5;
            }
            else if (dt <= 140)
            {
                gain = 2;
            }
        }

        int16_t scaled = (int16_t)abs(rawDelta) * gain;
        if (item.type == VALUE_FLOAT && scaled > 600)
        {
            scaled = 600;
        }
        return (rawDelta > 0) ? scaled : -scaled;
    }

    void nudgeValue(MenuItem &item, int16_t delta)
    {
        if (delta == 0)
        {
            return;
        }

        if (item.type == VALUE_BOOL)
        {
            bool &v = *static_cast<bool *>(item.ptr);
            if (delta > 0)
            {
                v = true;
            }
            else
            {
                v = false;
            }
            return;
        }

        if (item.type == VALUE_INT)
        {
            int &v = *static_cast<int *>(item.ptr);
            v += delta * (int)item.step;
            clampAndApply(item);
            return;
        }

        float &v = *static_cast<float *>(item.ptr);
        v += delta * item.step;
        clampAndApply(item);
    }

    float readAsFloat(const MenuItem &item)
    {
        if (item.type == VALUE_BOOL)
        {
            const bool &v = *static_cast<const bool *>(item.ptr);
            return v ? 1.0f : 0.0f;
        }

        if (item.type == VALUE_INT)
        {
            const int &v = *static_cast<const int *>(item.ptr);
            return (float)v;
        }

        return *static_cast<const float *>(item.ptr);
    }

    void writeFromFloat(const MenuItem &item, float value)
    {
        if (item.type == VALUE_BOOL)
        {
            bool &v = *static_cast<bool *>(item.ptr);
            v = value > 0.5f;
            return;
        }

        if (item.type == VALUE_INT)
        {
            int &v = *static_cast<int *>(item.ptr);
            v = (int)value;
            return;
        }

        float &v = *static_cast<float *>(item.ptr);
        v = value;
    }

    void formatValue(const MenuItem &item, char *buffer, size_t size)
    {
        if (item.type == VALUE_BOOL)
        {
            const bool &v = *static_cast<const bool *>(item.ptr);
            if (item.ptr == &MPPT_Mode)
            {
                snprintf(buffer, size, "%s", v ? "MPPT+CCCV" : "CCCV");
            }
            else if (item.ptr == &output_Mode)
            {
                snprintf(buffer, size, "%s", v ? "CHARGER" : "PSU");
            }
            else
            {
                snprintf(buffer, size, "%s", v ? "ON" : "OFF");
            }
            return;
        }

        if (item.ptr == &batteryPreset)
        {
            snprintf(buffer, size, "%s", getBatteryPresetName());
            return;
        }

        if (item.ptr == &oledDisplayMode)
        {
            const int mode = *static_cast<const int *>(item.ptr);
            snprintf(buffer, size, "%s", mode == 1 ? "CHARGER" : "DEV");
            return;
        }

        if (item.type == VALUE_INT)
        {
            const int &v = *static_cast<const int *>(item.ptr);
            snprintf(buffer, size, "%d", v);
            return;
        }

        const float &v = *static_cast<const float *>(item.ptr);
        snprintf(buffer, size, "%.*f", item.decimals, v);
    }

    void drawWiFiIcon(int x, int y)
    {
        // Signal bars are drawn bottom-aligned from left to right.
        int bars = 0;
        bool connected = false;

        if (enableWiFi && WiFi.status() == WL_CONNECTED)
        {
            connected = true;
            const long rssi = WiFi.RSSI();
            int quality = 2 * (rssi + 100);
            quality = constrain(quality, 0, 100);

            if (quality >= 75)
            {
                bars = 4;
            }
            else if (quality >= 50)
            {
                bars = 3;
            }
            else if (quality >= 25)
            {
                bars = 2;
            }
            else if (quality > 0)
            {
                bars = 1;
            }
        }

        const int baseY = y;
        const int barX[4] = {x, x + 3, x + 6, x + 9};
        const int barH[4] = {2, 4, 6, 8};
        for (int i = 0; i < 4; i++)
        {
            if (i < bars)
            {
                oled.drawBox(barX[i], baseY - barH[i], 2, barH[i]);
            }
            else
            {
                oled.drawFrame(barX[i], baseY - barH[i], 2, barH[i]);
            }
        }

        if (!connected)
        {
            // Slash the icon when Wi-Fi is disabled or disconnected.
            oled.drawLine(x - 1, baseY - 8, x + 11, baseY);
        }
    }

    void drawStatusPage()
    {
        char line[24];
        const bool compactChargerView = (oledDisplayMode == 1);
        const int totalPages = compactChargerView ? 1 : 4;
        const int activePage = compactChargerView ? 0 : statusPage;

        oled.clearBuffer();
        oled.setFont(u8g2_font_6x12_tf);
        snprintf(line, sizeof(line), "%s %d/%d", compactChargerView ? "CHARGER VIEW" : "MPPT STATUS", activePage + 1, totalPages);
        oled.drawStr(0, 10, line);
        drawWiFiIcon(114, 10);
        oled.drawLine(0, 12, 127, 12);

        if (compactChargerView)
        {
            snprintf(line, sizeof(line), "In  : %5.1fV %4.1fA", voltageInput, currentInput);
            oled.drawStr(0, 25, line);
            snprintf(line, sizeof(line), "Out : %5.1fV %4.1fA", voltageOutput, currentOutput);
            oled.drawStr(0, 38, line);
            snprintf(line, sizeof(line), "Pwr : %5.0fW %7.2fWh", powerInput, Wh);
            oled.drawStr(0, 51, line);
            snprintf(line, sizeof(line), "Run : %5.2fd Stage %d", daysRunning, chargingStage);
            oled.drawStr(0, 64, line);
        }
        else if (activePage == 0)
        {
            snprintf(line, sizeof(line), "In  : %5.1fV %4.1fA", voltageInput, currentInput);
            oled.drawStr(0, 25, line);
            snprintf(line, sizeof(line), "Out : %5.1fV %4.1fA", voltageOutput, currentOutput);
            oled.drawStr(0, 38, line);
            snprintf(line, sizeof(line), "Pwr : %5.0fW  Bat %3d%%", powerInput, batteryPercent);
            oled.drawStr(0, 51, line);
            snprintf(line, sizeof(line), "Tmp : %2dC   Stage %d", temperature, chargingStage);
            oled.drawStr(0, 64, line);
        }
        else if (activePage == 1)
        {
            snprintf(line, sizeof(line), "Wh  : %8.2f", Wh);
            oled.drawStr(0, 25, line);
            snprintf(line, sizeof(line), "kWh : %8.3f", kWh);
            oled.drawStr(0, 38, line);
            snprintf(line, sizeof(line), "Days: %8.2f", daysRunning);
            oled.drawStr(0, 51, line);
            snprintf(line, sizeof(line), "Mode: %s", MPPT_Mode ? "MPPT+CCCV" : "CCCV");
            oled.drawStr(0, 64, line);
        }
        else if (activePage == 2)
        {
            snprintf(line, sizeof(line), "ERR:%d REC:%d BNC:%d", ERR, REC, BNC);
            oled.drawStr(0, 25, line);
            snprintf(line, sizeof(line), "IUV:%d IOC:%d OOV:%d", IUV, IOC, OOV);
            oled.drawStr(0, 38, line);
            snprintf(line, sizeof(line), "OOC:%d OTE:%d FLV:%d", OOC, OTE, FLV);
            oled.drawStr(0, 51, line);
            snprintf(line, sizeof(line), "ADS:%d LCD:%d OLE:%d", ADS_Connected, LCD_Connected, OLED_Connected);
            oled.drawStr(0, 64, line);
        }
        else
        {
            snprintf(line, sizeof(line), "PWM:%4d PP:%4d EN:%d", PWM, PPWM, buckEnable);
            oled.drawStr(0, 25, line);
            snprintf(line, sizeof(line), "BYP:%d FAN:%d SRC:%d", bypassEnable, fanStatus, inputSource);
            oled.drawStr(0, 38, line);
            snprintf(line, sizeof(line), "SOC:%3d%% Temp:%2dC", batteryPercent, temperature);
            oled.drawStr(0, 51, line);
            snprintf(line, sizeof(line), "Loop:%6.2fms", loopTime);
            oled.drawStr(0, 64, line);
        }

        oled.sendBuffer();
    }

    void drawSettingsPage()
    {
        char value[24];

        oled.clearBuffer();
        oled.setFont(u8g2_font_6x12_tf);
        oled.drawStr(0, 10, editValue ? "SETTINGS [EDIT]" : "SETTINGS [NAV]");
        drawWiFiIcon(114, 10);
        oled.drawLine(0, 12, 127, 12);
        oled.drawVLine(82, 14, 50);

        for (int row = 0; row < 4; row++)
        {
            const int itemIndex = topIndex + row;
            if (itemIndex >= menuItemCount)
            {
                break;
            }

            const int y = 24 + row * 10;
            const MenuItem &item = menuItems[itemIndex];
            formatValue(item, value, sizeof(value));

            if (itemIndex == selectedIndex)
            {
                oled.drawBox(0, y - 9, 128, 10);
                oled.setDrawColor(0);
            }

            oled.drawStr(2, y, item.label);
            const int valueWidth = oled.getStrWidth(value);
            oled.drawStr(126 - valueWidth, y, value);

            if (itemIndex == selectedIndex)
            {
                oled.setDrawColor(1);
            }
        }

        oled.sendBuffer();
    }

    void ensureSelectionVisible()
    {
        if (selectedIndex < topIndex)
        {
            topIndex = selectedIndex;
        }
        else if (selectedIndex > topIndex + 3)
        {
            topIndex = selectedIndex - 3;
        }

        if (topIndex < 0)
        {
            topIndex = 0;
        }

        const int maxTop = menuItemCount - 4;
        if (topIndex > maxTop)
        {
            topIndex = maxTop;
        }

        if (topIndex < 0)
        {
            topIndex = 0;
        }
    }

    void commitRuntimeHooks()
    {
        clampAndApply(menuItems[2]);
        clampAndApply(menuItems[3]);
        clampAndApply(menuItems[4]);

        if (batteryPreset < 0)
        {
            batteryPreset = 0;
        }
        if (batteryPreset > 5)
        {
            batteryPreset = 5;
        }

        oledDisplayMode = constrain(oledDisplayMode, 0, 1);

        applyBatteryPreset(true);
        saveSettings();
    }

    void enterSettingsSafeState()
    {
        chargingPause = true;
        buck_Disable();
        bypassEnable = 0;
        digitalWrite(backflow_MOSFET, LOW);
    }

    void exitSettingsAndApply()
    {
        commitRuntimeHooks();
        chargingPause = false;
    }
}

void IO_Panel_Init()
{
    if (!OLED_Connected)
    {
        return;
    }

    pinMode(encoderPinA, encoderInputMode);
    pinMode(encoderPinB, encoderInputMode);
    pinMode(encoderPinSW, encoderInputMode);

    portENTER_CRITICAL(&encoderMux);
    encoderPrevAB = readEncoderAB();
    encoderQuarterSteps = 0;
    encoderDetentsPending = 0;
    portEXIT_CRITICAL(&encoderMux);

    attachInterrupt(digitalPinToInterrupt(encoderPinA), onEncoderEdge, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoderPinB), onEncoderEdge, CHANGE);

    oled.setBusClock(400000);
    oled.begin();
    oled.setPowerSave(0);
    oled.clearBuffer();
    oled.setFont(u8g2_font_6x12_tf);
    oled.drawStr(0, 14, "MPPT IO PANEL READY");
    oled.drawStr(0, 28, "Hold encoder to open");
    oled.drawStr(0, 42, "settings table menu");
    oled.sendBuffer();

    oledSleepEnabled = true;
    oledSleeping = false;
    lastUserActivityMs = millis();
}

void IO_Panel_Update()
{
    if (!OLED_Connected)
    {
        return;
    }

    bool swShort = false;
    bool swLong = false;

    updateEncoder();
    updateButton(encoderSw, encoderPinSW, swShort, swLong);

    const unsigned long now = millis();
    const bool userActivity = (encoderDelta != 0) || swShort || swLong || encoderSw.stablePressed;
    if (!oledSleepEnabled)
    {
        setOledSleepState(false);
    }
    else if (settingsOpen || userActivity)
    {
        lastUserActivityMs = now;
        setOledSleepState(false);
    }
    else if (now - lastUserActivityMs >= oledSleepTimeoutMs)
    {
        setOledSleepState(true);
    }

    if (swLong)
    {
        if (settingsOpen && editValue)
        {
            writeFromFloat(menuItems[selectedIndex], editSnapshot);
            editValue = false;
        }
        else
        {
            settingsOpen = !settingsOpen;
            editValue = false;
            encoderDelta = 0;
            if (settingsOpen)
            {
                enterSettingsSafeState();
            }
            else
            {
                exitSettingsAndApply();
            }
        }
        refreshRequested = true;
    }

    if (!settingsOpen)
    {
        chargingPause = false;

        if (encoderDelta != 0)
        {
            if (oledDisplayMode == 1)
            {
                statusPage = 0;
            }
            else
            {
                statusPage += (encoderDelta > 0) ? 1 : -1;
                if (statusPage > 3)
                {
                    statusPage = 0;
                }
                if (statusPage < 0)
                {
                    statusPage = 3;
                }
            }
            encoderDelta = 0;
            refreshRequested = true;
        }
    }
    else
    {
        chargingPause = true;

        if (encoderDelta != 0)
        {
            if (editValue)
            {
                nudgeValue(menuItems[selectedIndex], getAdaptiveEditDelta(menuItems[selectedIndex], encoderDelta));
            }
            else
            {
                selectedIndex += (encoderDelta > 0) ? 1 : -1;
                if (selectedIndex < 0)
                {
                    selectedIndex = menuItemCount - 1;
                }
                if (selectedIndex >= menuItemCount)
                {
                    selectedIndex = 0;
                }
                ensureSelectionVisible();
            }

            encoderDelta = 0;
            refreshRequested = true;
        }

        if (swShort)
        {
            if (!editValue)
            {
                editValue = true;
                editSnapshot = readAsFloat(menuItems[selectedIndex]);
                lastEditTurnMs = 0;
            }
            else
            {
                editValue = false;
            }
            refreshRequested = true;
        }

        // Single-switch mode: no AUX button. Use long press to cancel edit or close menu.
    }

    const unsigned long redrawIntervalMs = settingsOpen ? settingsRedrawIntervalMs : statusRedrawIntervalMs;
    if (!refreshRequested && (now - lastDrawMs < redrawIntervalMs))
    {
        return;
    }

    lastDrawMs = now;
    refreshRequested = false;

    if (oledSleeping)
    {
        return;
    }

    if (settingsOpen)
    {
        drawSettingsPage();
    }
    else
    {
        drawStatusPage();
    }
}
