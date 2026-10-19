#ifndef ELEKTRIAJAM_02_I_MOTOR_H
#define ELEKTRIAJAM_02_I_MOTOR_H

#include <stdint.h>

typedef enum {
  MOTOR_EDASI,
  MOTOR_TAGASI
} MotorDirection;

typedef enum {
  MOTOR_ALALISVOOL,
  MOTOR_SAMM
} MotorKind;

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
  void* context;
  MotorKind kind;
  void (*begin)(void* context);
  void (*update)(void* context);
  void (*start)(void* context);
  void (*stop)(void* context);
  void (*setSpeedPercent)(void* context, uint8_t percent);
  void (*setDirection)(void* context, MotorDirection direction);
  MotorStatus (*getStatus)(void* context);
} IMotor;

#endif
