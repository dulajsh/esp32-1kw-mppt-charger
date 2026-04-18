#ifndef BLYNK_TEMPLATE_ID
#define BLYNK_TEMPLATE_ID "TMP_FUGU_MPPT"
#endif
#ifndef BLYNK_TEMPLATE_NAME
#define BLYNK_TEMPLATE_NAME "FUGU MPPT"
#endif

#if __has_include(<BlynkSimpleEsp32.h>)
#include <BlynkSimpleEsp32.h>
#define HAS_BLYNK 1
#else
#define HAS_BLYNK 0
#endif

#include "telemetry.h"
#include "charging.h"

#if HAS_BLYNK
namespace
{
    const unsigned long wifiReconnectIntervalMs = 10000;
    const unsigned long blynkPublishIntervalMs = 200;
    unsigned long lastWifiReconnectAttemptMs = 0;
    unsigned long lastBlynkPublishMs = 0;

    bool isWiFiReady()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    void updateWiFiStatusFlag()
    {
        WIFI = isWiFiReady();
    }

    void ensureBlynkConnected()
    {
        if (!enableWiFi)
        {
            WIFI = 0;
            return;
        }

        updateWiFiStatusFlag();
        if (!isWiFiReady())
        {
            Blynk.disconnect();
            return;
        }

        if (Blynk.connected())
        {
            return;
        }

        const unsigned long now = millis();
        if (now - lastWifiReconnectAttemptMs < wifiReconnectIntervalMs)
        {
            return;
        }

        lastWifiReconnectAttemptMs = now;
        Blynk.connect(1000);
    }
}
#endif

namespace
{
    bool isSerialListenerActive()
    {
#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
        // Native USB CDC can reliably report terminal open state with DTR.
        return Serial && Serial.dtr();
#else
        // UART bridge boards cannot detect monitor-open reliably.
        // Keep telemetry off by default unless explicitly allowed.
#if ALLOW_UART_SERIAL_TELEMETRY
        return (bool)Serial;
#else
        return false;
#endif
#endif
    }
}

void Onboard_Telemetry()
{
    if (!isSerialListenerActive())
    {
        return;
    }

    const unsigned long serialIntervalMs = (millisSerialInterval < 20) ? 20 : (unsigned long)millisSerialInterval;

    currentSerialMillis = millis();
    if (currentSerialMillis - prevSerialMillis >= serialIntervalMs)
    {
        prevSerialMillis = currentSerialMillis;

        if (serialTelemMode == 0)
        {
        }
        else if (serialTelemMode == 1)
        {
            Serial.print(" ERR:");
            Serial.print(ERR);
            Serial.print(" FLV:");
            Serial.print(FLV);
            Serial.print(" BNC:");
            Serial.print(BNC);
            Serial.print(" IUV:");
            Serial.print(IUV);
            Serial.print(" IOC:");
            Serial.print(IOC);
            Serial.print(" OOV:");
            Serial.print(OOV);
            Serial.print(" OOC:");
            Serial.print(OOC);
            Serial.print(" OTE:");
            Serial.print(OTE);
            Serial.print(" REC:");
            Serial.print(REC);
            Serial.print(" MPPTA:");
            Serial.print(MPPT_Mode);
            Serial.print(" CM:");
            Serial.print(output_Mode);
            Serial.print(" BST:");
            Serial.print(getChargingStageName());
            Serial.print(" BPRE:");
            Serial.print(getBatteryPresetName());

            Serial.print(" ");
            Serial.print(" BYP:");
            Serial.print(bypassEnable);
            Serial.print(" EN:");
            Serial.print(buckEnable);
            Serial.print(" FAN:");
            Serial.print(fanStatus);
            Serial.print(" WiFi:");
            Serial.print(WIFI);
            Serial.print(" ");
            Serial.print(" PI:");
            Serial.print(powerInput, 0);
            Serial.print(" PWM:");
            Serial.print(PWM);
            Serial.print(" PPWM:");
            Serial.print(PPWM);
            Serial.print(" VI:");
            Serial.print(voltageInput, 1);
            Serial.print(" VO:");
            Serial.print(voltageOutput, 1);
            Serial.print(" CI:");
            Serial.print(currentInput, 2);
            Serial.print(" CO:");
            Serial.print(currentOutput, 2);
            Serial.print(" Wh:");
            Serial.print(Wh, 2);
            Serial.print(" Temp:");
            Serial.print(temperature, 1);
            Serial.print(" ");
            Serial.print(" CSMPV:");
            Serial.print(currentMidPoint, 3);
            Serial.print(" CSV:");
            Serial.print(CSI_converted, 3);
            Serial.print(" VO%Dev:");
            Serial.print(outputDeviation, 1);
            Serial.print(" SOC:");
            Serial.print(batteryPercent);
            Serial.print("%");
            Serial.print(" T:");
            Serial.print(secondsElapsed);
            Serial.print(" LoopT:");
            Serial.print(loopTime, 3);
            Serial.print("ms");
            Serial.println("");
        }
        else if (serialTelemMode == 2)
        {
            Serial.print(" PI:");
            Serial.print(powerInput, 0);
            Serial.print(" PWM:");
            Serial.print(PWM);
            Serial.print(" PPWM:");
            Serial.print(PPWM);
            Serial.print(" VI:");
            Serial.print(voltageInput, 1);
            Serial.print(" VO:");
            Serial.print(voltageOutput, 1);
            Serial.print(" CI:");
            Serial.print(currentInput, 2);
            Serial.print(" CO:");
            Serial.print(currentOutput, 2);
            Serial.print(" Wh:");
            Serial.print(Wh, 2);
            Serial.print(" Temp:");
            Serial.print(temperature, 1);
            Serial.print(" EN:");
            Serial.print(buckEnable);
            Serial.print(" FAN:");
            Serial.print(fanStatus);
            Serial.print(" ST:");
            Serial.print(getChargingStageName());
            Serial.print(" SOC:");
            Serial.print(batteryPercent);
            Serial.print("%");
            Serial.print(" T:");
            Serial.print(secondsElapsed);
            Serial.print(" LoopT:");
            Serial.print(loopTime, 3);
            Serial.print("ms");
            Serial.println("");
        }
        else if (serialTelemMode == 3)
        {
            Serial.print(" ");
            Serial.print(powerInput, 0);
            Serial.print(" ");
            Serial.print(voltageInput, 1);
            Serial.print(" ");
            Serial.print(voltageOutput, 1);
            Serial.print(" ");
            Serial.print(currentInput, 2);
            Serial.print(" ");
            Serial.print(currentOutput, 2);
            Serial.print(" ");
            Serial.print(Wh, 2);
            Serial.print(" ");
            Serial.print(temperature, 1);
            Serial.print(" ");
            Serial.print(buckEnable);
            Serial.print(" ");
            Serial.print(fanStatus);
            Serial.print(" ");
            Serial.print(batteryPercent);
            Serial.print(" ");
            Serial.print(secondsElapsed);
            Serial.print(" ");
            Serial.print(loopTime, 3);
            Serial.print(" ");
            Serial.println("");
        }
    }
}

void setupWiFi()
{
    if (enableWiFi == 1)
    {
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(ssid, pass);
        WIFI = 0;

#if HAS_BLYNK
        Blynk.config(auth);
#endif
    }
    else
    {
        WIFI = 0;
    }
}

void Wireless_Telemetry()
{
    if (enableWiFi == 1)
    {
        WIFI = (WiFi.status() == WL_CONNECTED);

#if HAS_BLYNK
        ensureBlynkConnected();

        if (!Blynk.connected())
        {
            return;
        }

        Blynk.run();

        const unsigned long now = millis();
        if (now - lastBlynkPublishMs < blynkPublishIntervalMs)
        {
            return;
        }
        lastBlynkPublishMs = now;

        int LED1, LED2, LED3, LED4;
        if (buckEnable == 1)
        {
            LED1 = 200;
        }
        else
        {
            LED1 = 0;
        }
        if (batteryPercent >= 99)
        {
            LED2 = 200;
        }
        else
        {
            LED2 = 0;
        }
        if (batteryPercent <= 10)
        {
            LED3 = 200;
        }
        else
        {
            LED3 = 0;
        }
        if (IUV == 0)
        {
            LED4 = 200;
        }
        else
        {
            LED4 = 0;
        }

        Blynk.virtualWrite(V1, powerInput);
        Blynk.virtualWrite(V2, batteryPercent);
        Blynk.virtualWrite(V3, voltageInput);
        Blynk.virtualWrite(V4, currentInput);
        Blynk.virtualWrite(V5, voltageOutput);
        Blynk.virtualWrite(V6, currentOutput);
        Blynk.virtualWrite(V7, temperature);
        Blynk.virtualWrite(V8, Wh / 1000);
        Blynk.virtualWrite(V9, energySavings);
        Blynk.virtualWrite(V10, LED1);
        Blynk.virtualWrite(V11, LED2);
        Blynk.virtualWrite(V12, LED3);
        Blynk.virtualWrite(V13, LED4);

        Blynk.virtualWrite(V14, voltageBatteryMin);
        Blynk.virtualWrite(V15, voltageBatteryMax);
        Blynk.virtualWrite(V16, currentCharging);
        Blynk.virtualWrite(V17, electricalPrice);
#endif
    }

    if (enableBluetooth == 1)
    {
        // ADD BLUETOOTH CODE
    }
}
