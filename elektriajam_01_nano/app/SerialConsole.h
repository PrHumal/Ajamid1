#ifndef ELEKTRIAJAM_SERIAL_CONSOLE_H
#define ELEKTRIAJAM_SERIAL_CONSOLE_H

#include <Arduino.h>
#include "../devices/IMotor.h"

void consoleBegin(IMotor* motor, unsigned long baud);
void consoleUpdate(void);

#endif
