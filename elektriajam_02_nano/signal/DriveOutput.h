#ifndef ELEKTRIAJAM_02_DRIVE_OUTPUT_H
#define ELEKTRIAJAM_02_DRIVE_OUTPUT_H

#include <stdint.h>
#include "../devices/IMotor.h"

typedef struct {
  void* context;
  void (*begin)(void* context);
  void (*setDirection)(void* context, MotorDirection direction);
  void (*drive)(void* context, uint8_t value);
  void (*off)(void* context);
  void (*brake)(void* context);
  void (*step)(void* context);
  void (*service)(void* context);
} DriveOutput;

#endif
