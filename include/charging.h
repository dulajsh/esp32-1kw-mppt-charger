#ifndef CHARGING_H
#define CHARGING_H

#include "config.h"

void buck_Enable();
void buck_Disable();
void predictivePWM();
void PWM_Modulation();
void Charging_Algorithm();
void applyBatteryPreset(bool forceRefresh);
const char *getChargingStageName();
const char *getBatteryPresetName();

#endif
