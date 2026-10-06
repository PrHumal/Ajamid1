#ifndef ELEKTRIAJAM_I_MOTOR_H
#define ELEKTRIAJAM_I_MOTOR_H

#include <stdint.h>

typedef enum {
  MOTOR_EDASI,
  MOTOR_TAGASI
} MotorDirection;

typedef enum {
  MOTOR_SEISATUD,
  MOTOR_TOIMIB,
  MOTOR_SUUNAVAHETUS,
  MOTOR_PIDURDATUD
} MotorState;

typedef struct {
  MotorState state;
  MotorDirection direction;
  uint8_t targetPercent;
  uint8_t appliedPercent;
} MotorStatus;

typedef struct {
  void (*begin)(void);
  void (*update)(void);
  void (*start)(void);
  void (*stop)(void);
  void (*setSpeedPercent)(uint8_t percent);
  void (*setDirection)(MotorDirection direction);
  MotorStatus (*getStatus)(void);
} IMotor;

#endif
