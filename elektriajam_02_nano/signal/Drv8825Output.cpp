#include "Drv8825Output.h"
#include "../mcu/NanoPins.h"
#include <Arduino.h>

static uint8_t stepHigh;
static uint32_t stepTransition;

static void drvBegin(void*) {
  pinMode(NANO_DRV_STEP, OUTPUT);
  pinMode(NANO_DRV_DIR, OUTPUT);
  pinMode(NANO_DRV_ENBL, OUTPUT);
  digitalWrite(NANO_DRV_STEP, LOW);
  digitalWrite(NANO_DRV_DIR, LOW);
  digitalWrite(NANO_DRV_ENBL, HIGH);
}

static void drvSetDirection(void*, MotorDirection direction) {
  digitalWrite(NANO_DRV_DIR, direction == MOTOR_EDASI ? HIGH : LOW);
}

static void drvDrive(void*, uint8_t) {
  digitalWrite(NANO_DRV_ENBL, LOW);
}

static void drvOff(void*) {
  digitalWrite(NANO_DRV_STEP, LOW);
  digitalWrite(NANO_DRV_ENBL, HIGH);
  stepHigh = 0;
}

static void drvBrake(void* context) { drvOff(context); }

static void drvStep(void*) {
  if (!stepHigh) {
    digitalWrite(NANO_DRV_STEP, HIGH);
    stepHigh = 1;
    stepTransition = micros();
  }
}

static void drvService(void*) {
  if (stepHigh && (uint32_t)(micros() - stepTransition) >= 2UL) {
    digitalWrite(NANO_DRV_STEP, LOW);
    stepHigh = 0;
  }
}

void drv8825Create(DriveOutput* output) {
  output->context = 0;
  output->begin = drvBegin;
  output->setDirection = drvSetDirection;
  output->drive = drvDrive;
  output->off = drvOff;
  output->brake = drvBrake;
  output->step = drvStep;
  output->service = drvService;
}
