#ifndef ELEKTRIAJAM_I_DRIVE_OUTPUT_H
#define ELEKTRIAJAM_I_DRIVE_OUTPUT_H

#include <stdint.h>
#include "../devices/IMotor.h"

typedef struct {
  void (*begin)(void);
  void (*setDirection)(MotorDirection direction);
  void (*drive)(uint8_t pwm);
  void (*off)(void);
  void (*brake)(void);
} IDriveOutput;

#endif
