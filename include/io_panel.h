#ifndef IO_PANEL_H
#define IO_PANEL_H

#include "config.h"

void IO_Panel_Init();
void IO_Panel_Update();
void IO_Panel_ShowOTAProgress(unsigned int percent, int state);

#endif
