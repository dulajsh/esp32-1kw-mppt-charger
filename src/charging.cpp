#include "charging.h"
#include "sensors.h"

namespace
{
enum BatteryPreset
{
    PRESET_CUSTOM = 0,
    PRESET_LEAD_ACID = 1,
    PRESET_AGM = 2,
    PRESET_GEL = 3,
    PRESET_LIFEPO4 = 4,
    PRESET_LI_ION = 5
};

enum ChargingStage
{
    STAGE_BULK = 0,
    STAGE_ABSORPTION = 1,
    STAGE_FLOAT = 2
};

void setChargingStage(ChargingStage stage)
{
    if (chargingStage != stage)
    {
        chargingStage = stage;
        chargingStageStartMillis = millis();
    }
}

void refreshCustomProfileTargets()
{
    batteryAbsorptionVoltage = voltageBatteryMax;
    batteryFloatVoltage = voltageBatteryMax * 0.9670;
    batteryRechargeVoltage = voltageBatteryMax * 0.9240;
    batteryAbsExitCurrent = max(0.5f, currentCharging * 0.10f);
    batteryFloatEnabled = true;
    batteryAbsorptionTimeoutMs = 7200000;
}

void updateStageTargets()
{
    if (chargingStage == STAGE_BULK)
    {
        chargeVoltageTarget = batteryAbsorptionVoltage;
        chargeCurrentTarget = currentCharging;
    }
    else if (chargingStage == STAGE_ABSORPTION)
    {
        chargeVoltageTarget = batteryAbsorptionVoltage;
        chargeCurrentTarget = max(batteryAbsExitCurrent, currentCharging * 0.50f);
    }
    else
    {
        chargeVoltageTarget = batteryFloatEnabled ? batteryFloatVoltage : batteryAbsorptionVoltage;
        chargeCurrentTarget = max(batteryAbsExitCurrent, currentCharging * 0.20f);
    }
}

void updateChargingStageTransitions()
{
    unsigned long stageElapsed = millis() - chargingStageStartMillis;

    if (chargingStage == STAGE_BULK)
    {
        if (voltageOutput >= batteryAbsorptionVoltage - 0.15f)
        {
            setChargingStage(STAGE_ABSORPTION);
        }
    }
    else if (chargingStage == STAGE_ABSORPTION)
    {
        bool currentTapered = (currentOutput <= batteryAbsExitCurrent) && (voltageOutput >= batteryAbsorptionVoltage - 0.10f);
        bool absorbTimedOut = stageElapsed >= batteryAbsorptionTimeoutMs;
        if (currentTapered || absorbTimedOut)
        {
            setChargingStage(STAGE_FLOAT);
        }
    }
    else
    {
        if (voltageOutput <= batteryRechargeVoltage)
        {
            setChargingStage(STAGE_BULK);
        }
    }

    updateStageTargets();
}
} // namespace

void buck_Enable()
{
    buckEnable = 1;
    digitalWrite(buck_EN, HIGH);
    digitalWrite(LED, HIGH);
}

void applyBatteryPreset(bool forceRefresh)
{
    static int lastPreset = -1;
    static float lastVmax = -1.0f;
    static float lastVmin = -1.0f;
    static float lastCurrent = -1.0f;

    bool presetChanged = (batteryPreset != lastPreset);
    bool customInputsChanged = (batteryPreset == PRESET_CUSTOM) &&
                               ((voltageBatteryMax != lastVmax) || (voltageBatteryMin != lastVmin) || (currentCharging != lastCurrent));

    if (!(forceRefresh || presetChanged || customInputsChanged))
    {
        return;
    }

    if (batteryPreset == PRESET_LEAD_ACID)
    {
        voltageBatteryMax = 28.80f;
        voltageBatteryMin = 22.40f;
        currentCharging = 25.00f;
        batteryAbsorptionVoltage = 28.80f;
        batteryFloatVoltage = 27.20f;
        batteryRechargeVoltage = 25.20f;
        batteryAbsExitCurrent = 2.50f;
        batteryFloatEnabled = true;
        batteryAbsorptionTimeoutMs = 10800000;
    }
    else if (batteryPreset == PRESET_AGM)
    {
        voltageBatteryMax = 28.40f;
        voltageBatteryMin = 22.80f;
        currentCharging = 24.00f;
        batteryAbsorptionVoltage = 28.40f;
        batteryFloatVoltage = 27.00f;
        batteryRechargeVoltage = 25.00f;
        batteryAbsExitCurrent = 2.20f;
        batteryFloatEnabled = true;
        batteryAbsorptionTimeoutMs = 9000000;
    }
    else if (batteryPreset == PRESET_GEL)
    {
        voltageBatteryMax = 28.20f;
        voltageBatteryMin = 22.80f;
        currentCharging = 20.00f;
        batteryAbsorptionVoltage = 28.20f;
        batteryFloatVoltage = 27.00f;
        batteryRechargeVoltage = 25.40f;
        batteryAbsExitCurrent = 1.80f;
        batteryFloatEnabled = true;
        batteryAbsorptionTimeoutMs = 7200000;
    }
    else if (batteryPreset == PRESET_LIFEPO4)
    {
        voltageBatteryMax = 28.40f;
        voltageBatteryMin = 24.00f;
        currentCharging = 30.00f;
        batteryAbsorptionVoltage = 28.40f;
        batteryFloatVoltage = 27.00f;
        batteryRechargeVoltage = 25.60f;
        batteryAbsExitCurrent = 3.00f;
        batteryFloatEnabled = true;
        batteryAbsorptionTimeoutMs = 3600000;
    }
    else if (batteryPreset == PRESET_LI_ION)
    {
        voltageBatteryMax = 29.40f;
        voltageBatteryMin = 21.00f;
        currentCharging = 20.00f;
        batteryAbsorptionVoltage = 29.40f;
        batteryFloatVoltage = 28.00f;
        batteryRechargeVoltage = 26.00f;
        batteryAbsExitCurrent = 1.60f;
        batteryFloatEnabled = true;
        batteryAbsorptionTimeoutMs = 3600000;
    }
    else
    {
        refreshCustomProfileTargets();
    }

    updateStageTargets();
    lastPreset = batteryPreset;
    lastVmax = voltageBatteryMax;
    lastVmin = voltageBatteryMin;
    lastCurrent = currentCharging;
}

const char *getChargingStageName()
{
    if (chargingStage == STAGE_BULK)
    {
        return "BULK";
    }
    if (chargingStage == STAGE_ABSORPTION)
    {
        return "ABS";
    }
    return "FLOAT";
}

const char *getBatteryPresetName()
{
    if (batteryPreset == PRESET_LEAD_ACID)
    {
        return "LEAD";
    }
    if (batteryPreset == PRESET_AGM)
    {
        return "AGM";
    }
    if (batteryPreset == PRESET_GEL)
    {
        return "GEL";
    }
    if (batteryPreset == PRESET_LIFEPO4)
    {
        return "LIFEPO4";
    }
    if (batteryPreset == PRESET_LI_ION)
    {
        return "LI-ION";
    }
    return "CUSTOM";
}

void buck_Disable()
{
    buckEnable = 0;
    digitalWrite(buck_EN, LOW);
    digitalWrite(LED, LOW);
    PWM = 0;
}

void predictivePWM()
{
    if (voltageInput <= 0)
    {
        PPWM = 0;
    }
    else
    {
        PPWM = (PPWM_margin * pwmMax * voltageOutput) / (100.00 * voltageInput);
    }
    PPWM = constrain(PPWM, 0, pwmMaxLimited);
}

void PWM_Modulation()
{
    if (output_Mode == 0)
    {
        PWM = constrain(PWM, 0, pwmMaxLimited);
    }
    else
    {
        predictivePWM();
        PWM = constrain(PWM, PPWM, pwmMaxLimited);
    }
    ledcWrite(pwmChannel, PWM);
    buck_Enable();
}

void Charging_Algorithm()
{
    applyBatteryPreset(false);
    updateChargingStageTransitions();

    if (ERR > 0 || chargingPause == 1)
    {
        buck_Disable();
    }
    else
    {
        if (REC == 1)
        {
            REC = 0;
            buck_Disable();
            lcd.setCursor(0, 0);
            lcd.print("POWER SOURCE    ");
            lcd.setCursor(0, 1);
            lcd.print("DETECTED        ");
            Serial.println("> Solar Panel Detected");
            Serial.print("> Computing For Predictive PWM ");
            for (int i = 0; i < 40; i++)
            {
                Serial.print(".");
                delay(30);
            }
            Serial.println("");
            Read_Sensors();
            predictivePWM();
            PWM = PPWM;
            setChargingStage(STAGE_BULK);
            updateStageTargets();
            lcd.clear();
        }
        else
        {
            if (MPPT_Mode == 0)
            {
                if (currentOutput > chargeCurrentTarget)
                {
                    PWM--;
                }
                else if (voltageOutput > chargeVoltageTarget)
                {
                    PWM--;
                }
                else if (voltageOutput < chargeVoltageTarget)
                {
                    PWM++;
                }
                else
                {
                }
                PWM_Modulation();
            }
            else
            {
                if (currentOutput > chargeCurrentTarget)
                {
                    PWM--;
                }
                else if (voltageOutput > chargeVoltageTarget)
                {
                    PWM--;
                }
                else
                {
                    if (powerInput > powerInputPrev && voltageInput > voltageInputPrev)
                    {
                        PWM--;
                    }
                    else if (powerInput > powerInputPrev && voltageInput < voltageInputPrev)
                    {
                        PWM++;
                    }
                    else if (powerInput < powerInputPrev && voltageInput > voltageInputPrev)
                    {
                        PWM++;
                    }
                    else if (powerInput < powerInputPrev && voltageInput < voltageInputPrev)
                    {
                        PWM--;
                    }
                    else if (voltageOutput < chargeVoltageTarget)
                    {
                        PWM++;
                    }
                    powerInputPrev = powerInput;
                    voltageInputPrev = voltageInput;
                }
                PWM_Modulation();
            }
        }
    }
}
