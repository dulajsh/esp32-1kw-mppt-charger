#include "protection.h"

void backflowControl()
{
    if (chargingPause == 1)
    {
        bypassEnable = 0;
        digitalWrite(backflow_MOSFET, LOW);
        return;
    }

    if (output_Mode == 0)
    {
        bypassEnable = 1;
    }
    else
    {
        if (voltageInput > voltageOutput + voltageDropout)
        {
            bypassEnable = 1;
        }
        else
        {
            bypassEnable = 0;
        }
    }
    digitalWrite(backflow_MOSFET, bypassEnable);
}

void Device_Protection()
{
    currentRoutineMillis = millis();
    if (currentErrorMillis - prevErrorMillis >= errorTimeLimit)
    {
        prevErrorMillis = currentErrorMillis;
        if (errorCount < errorCountLimit)
        {
            errorCount = 0;
        }
    }

    ERR = 0;
    backflowControl();
    if (temperature > temperatureMax)
    {
        OTE = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        OTE = 0;
    }
    if (currentInput > currentInAbsolute)
    {
        IOC = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        IOC = 0;
    }
    if (currentOutput > currentOutAbsolute)
    {
        OOC = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        OOC = 0;
    }
    if (voltageOutput > voltageBatteryMax + voltageBatteryThresh)
    {
        OOV = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        OOV = 0;
    }
    if (voltageInput < vInSystemMin && voltageOutput < vInSystemMin)
    {
        FLV = 1;
        ERR++;
        errorCount++;
    }
    else
    {
        FLV = 0;
    }

    if (output_Mode == 0)
    {
        REC = 0;
        BNC = 0;
        if (voltageInput < voltageBatteryMax + voltageDropout)
        {
            IUV = 1;
            ERR++;
            errorCount++;
        }
        else
        {
            IUV = 0;
        }
    }
    else
    {
        backflowControl();
        if (voltageOutput < vInSystemMin)
        {
            BNC = 1;
            ERR++;
        }
        else
        {
            BNC = 0;
        }
        if (voltageInput < voltageBatteryMax + voltageDropout)
        {
            IUV = 1;
            ERR++;
            REC = 1;
        }
        else
        {
            IUV = 0;
        }
    }
}
