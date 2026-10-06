#ifndef ELEKTRIAJAM_MOTOR_CONTROLLER_H
#define ELEKTRIAJAM_MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "../devices/IMotor.h"
#include "../signal/IDriveOutput.h"

typedef struct {
  IDriveOutput* output;
  MotorDirection direction;
  MotorDirection pendingDirection;
  uint8_t targetPercent;
  float appliedPercent;
  uint8_t running;
  uint8_t stopping;
  uint8_t changingDirection;
  uint8_t braking;
  uint32_t lastUpdate;
} MotorController;

void motorControllerCreate(MotorController* controller, IDriveOutput* output);
IMotor* motorControllerInterface(MotorController* controller);
void motorControllerBegin(void);
void motorControllerUpdate(void);
void motorControllerStart(void);
void motorControllerStop(void);
void motorControllerSetSpeed(uint8_t percent);
void motorControllerSetDirection(MotorDirection direction);
MotorStatus motorControllerStatus(void);

#endif
