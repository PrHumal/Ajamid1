#ifndef ELEKTRIAJAM_02_MOTOR_CONTROLLER_H
#define ELEKTRIAJAM_02_MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "../devices/IMotor.h"
#include "../signal/DriveOutput.h"

typedef struct {
  DriveOutput* output;
  MotorKind kind;
  MotorDirection direction;
  MotorDirection pendingDirection;
  uint8_t targetPercent;
  float appliedPercent;
  uint8_t running;
  uint8_t stopping;
  uint8_t changingDirection;
  uint8_t braking;
  uint32_t lastUpdate;
  uint32_t lastStep;
} MotorController;

void motorControllerCreate(MotorController* controller, DriveOutput* output,
                            MotorKind kind);
void motorControllerInterface(MotorController* controller, IMotor* motor);

#endif
