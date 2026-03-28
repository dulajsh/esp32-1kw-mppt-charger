#include "charging.h"
#include "sensors.h"

void buck_Enable()
{
    buckEnable = 1;
    digitalWrite(buck_EN, HIGH);
    digitalWrite(LED, HIGH);
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
            lcd.clear();
        }
        else
        {
            if (MPPT_Mode == 0)
            {
                if (currentOutput > currentCharging)
                {
                    PWM--;
                }
                else if (voltageOutput > voltageBatteryMax)
                {
                    PWM--;
                }
                else if (voltageOutput < voltageBatteryMax)
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
                if (currentOutput > currentCharging)
                {
                    PWM--;
                }
                else if (voltageOutput > voltageBatteryMax)
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
                    else if (voltageOutput < voltageBatteryMax)
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
