#include "Vnh5019Output.h"
#include "../mcu/NanoPins.h"
#include <Arduino.h>

static MotorDirection direction = MOTOR_EDASI;

static void vnhBegin(void*) {
  pinMode(NANO_VNH_INA, OUTPUT);
  pinMode(NANO_VNH_INB, OUTPUT);
  pinMode(NANO_VNH_PWM, OUTPUT);
  digitalWrite(NANO_VNH_INA, LOW);
  digitalWrite(NANO_VNH_INB, LOW);
  analogWrite(NANO_VNH_PWM, 0);
}

static void vnhSetDirection(void*, MotorDirection value) {
  direction = value;
  digitalWrite(NANO_VNH_INA, value == MOTOR_EDASI ? HIGH : LOW);
  digitalWrite(NANO_VNH_INB, value == MOTOR_EDASI ? LOW : HIGH);
}

static void vnhDrive(void*, uint8_t pwm) {
  vnhSetDirection(0, direction);
  analogWrite(NANO_VNH_PWM, pwm);
}

static void vnhOff(void*) {
  digitalWrite(NANO_VNH_INA, LOW);
  digitalWrite(NANO_VNH_INB, LOW);
  analogWrite(NANO_VNH_PWM, 0);
}

static void vnhBrake(void*) {
  digitalWrite(NANO_VNH_INA, LOW);
  digitalWrite(NANO_VNH_INB, LOW);
  analogWrite(NANO_VNH_PWM, 255);
}

static void vnhStep(void*) {}
static void vnhService(void*) {}

void vnhCreate(DriveOutput* output) {
  output->context = 0;
  output->begin = vnhBegin;
  output->setDirection = vnhSetDirection;
  output->drive = vnhDrive;
  output->off = vnhOff;
  output->brake = vnhBrake;
  output->step = vnhStep;
  output->service = vnhService;
}
