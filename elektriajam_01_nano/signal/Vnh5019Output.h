#ifndef ELEKTRIAJAM_VNH5019_OUTPUT_H
#define ELEKTRIAJAM_VNH5019_OUTPUT_H

#include <Arduino.h>
#include "IDriveOutput.h"

void vnhBegin(void);
void vnhSetDirection(MotorDirection direction);
void vnhDrive(uint8_t pwm);
void vnhOff(void);
void vnhBrake(void);
void vnhCreate(IDriveOutput* output);

#endif
