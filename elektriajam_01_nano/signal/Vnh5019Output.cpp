#include "Vnh5019Output.h"
#include "../mcu/NanoPins.h"

static MotorDirection currentDirection = MOTOR_EDASI;

void vnhBegin(void) {
  pinMode(NANO_INA, OUTPUT);
  pinMode(NANO_INB, OUTPUT);
  pinMode(NANO_PWM, OUTPUT);
  vnhOff();
}

void vnhSetDirection(MotorDirection direction) {
  currentDirection = direction;
  digitalWrite(NANO_INA, direction == MOTOR_EDASI ? HIGH : LOW);
  digitalWrite(NANO_INB, direction == MOTOR_EDASI ? LOW : HIGH);
}

void vnhDrive(uint8_t pwm) {
  vnhSetDirection(currentDirection);
  analogWrite(NANO_PWM, pwm);
}

void vnhOff(void) {
  digitalWrite(NANO_INA, LOW);
  digitalWrite(NANO_INB, LOW);
  analogWrite(NANO_PWM, 0);
}

void vnhBrake(void) {
  digitalWrite(NANO_INA, LOW);
  digitalWrite(NANO_INB, LOW);
  analogWrite(NANO_PWM, 255);
}

void vnhCreate(IDriveOutput* output) {
  output->begin = vnhBegin;
  output->setDirection = vnhSetDirection;
  output->drive = vnhDrive;
  output->off = vnhOff;
  output->brake = vnhBrake;
}
