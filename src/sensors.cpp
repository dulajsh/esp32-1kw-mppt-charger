#include "sensors.h"

void ADC_SetGain()
{
    if (ADS1015_Mode == true)
    {
        if (ADC_GainSelect == 0)
        {
            ads.setGain(GAIN_TWOTHIRDS);
            ADC_BitReso = 3.0000;
        }
        else if (ADC_GainSelect == 1)
        {
            ads.setGain(GAIN_ONE);
            ADC_BitReso = 2.0000;
        }
        else if (ADC_GainSelect == 2)
        {
            ads.setGain(GAIN_TWO);
            ADC_BitReso = 1.0000;
        }
    }
    else
    {
        if (ADC_GainSelect == 0)
        {
            ads.setGain(GAIN_TWOTHIRDS);
            ADC_BitReso = 0.1875;
        }
        else if (ADC_GainSelect == 1)
        {
            ads.setGain(GAIN_ONE);
            ADC_BitReso = 0.125;
        }
        else if (ADC_GainSelect == 2)
        {
            ads.setGain(GAIN_TWO);
            ADC_BitReso = 0.0625;
        }
    }
}

void resetVariables()
{
    secondsElapsed = 0;
    energySavings = 0;
    daysRunning = 0;
    timeOn = 0;
}

void Read_Sensors()
{
    if (sampleStoreTS <= avgCountTS)
    {
        TS = TS + analogRead(TempSensor);
        sampleStoreTS++;
    }
    else
    {
        TS = TS / sampleStoreTS;
        TSlog = log(ntcResistance * (4095.00 / TS - 1.00));
        temperature = (1.0 / (1.009249522e-03 + 2.378405444e-04 * TSlog + 2.019202697e-07 * TSlog * TSlog * TSlog)) - 273.15;
        sampleStoreTS = 0;
        TS = 0;
    }

    VSI = 0.0000;
    VSO = 0.0000;
    CSI = 0.0000;

    if (ADS_Connected)
    {
        for (int i = 0; i < avgCountVS; i++)
        {
            VSI = VSI + ads.computeVolts(ads.readADC_SingleEnded(3));
            VSO = VSO + ads.computeVolts(ads.readADC_SingleEnded(1));
        }
        voltageInput = ((VSI / avgCountVS) * inVoltageDivRatio) + inVoltageOffset;
        voltageOutput = ((VSO / avgCountVS) * outVoltageDivRatio) + outVoltageOffset;

        for (int i = 0; i < avgCountCS; i++)
        {
            CSI = CSI + ads.computeVolts(ads.readADC_SingleEnded(2));
        }
        CSI_converted = (CSI / avgCountCS) * 1.3300;
        currentInput = ((CSI_converted - currentMidPoint) * -1) / currentSensV;
        if (currentInput < 0)
        {
            currentInput = 0.0000;
        }
        if (voltageOutput <= 0)
        {
            currentOutput = 0.0000;
        }
        else
        {
            currentOutput = (voltageInput * currentInput) / voltageOutput;
        }
    }
    else
    {
        voltageInput = 0.0000;
        voltageOutput = 0.0000;
        currentInput = 0.0000;
        currentOutput = 0.0000;
        CSI_converted = 0.0000;
    }

    if (voltageInput <= 3 && voltageOutput <= 3)
    {
        inputSource = 0;
    }
    else if (voltageInput > voltageOutput)
    {
        inputSource = 1;
    }
    else if (voltageInput < voltageOutput)
    {
        inputSource = 2;
    }

    if (ADS_Connected && buckEnable == 0 && FLV == 0 && OOV == 0)
    {
        currentMidPoint = ((CSI / avgCountCS) * 1.3300) - 0.003;
    }

    powerInput = voltageInput * currentInput;
    powerOutput = voltageInput * currentInput * efficiencyRate;
    outputDeviation = (voltageOutput / voltageBatteryMax) * 100.000;

    batteryPercent = ((voltageOutput - voltageBatteryMin) / (voltageBatteryMax - voltageBatteryMin)) * 101;
    batteryPercent = constrain(batteryPercent, 0, 100);

    currentRoutineMillis = millis();
    if (currentRoutineMillis - prevRoutineMillis >= millisRoutineInterval)
    {
        prevRoutineMillis = currentRoutineMillis;
        Wh = Wh + (powerInput / (3600.000 * (1000.000 / millisRoutineInterval)));
        kWh = Wh / 1000.000;
        MWh = Wh / 1000000.000;
        daysRunning = timeOn / (86400.000 * (1000.000 / millisRoutineInterval));
        timeOn++;
    }

    secondsElapsed = millis() / 1000;
    energySavings = electricalPrice * (Wh / 1000.0000);
}
