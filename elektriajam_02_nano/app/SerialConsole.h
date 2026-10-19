#ifndef ELEKTRIAJAM_02_SERIAL_CONSOLE_H
#define ELEKTRIAJAM_02_SERIAL_CONSOLE_H

#include <Arduino.h>
#include "../devices/IMotor.h"

void consoleBegin(IMotor* dc, IMotor* stepper, unsigned long baud);
void consoleUpdate(void);

#endif
