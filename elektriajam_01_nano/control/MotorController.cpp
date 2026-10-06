#include "MotorController.h"

static MotorController* activeController;
static IMotor motorInterface;

static uint8_t percentToPwm(float percent) {
  return (uint8_t)(percent * 255.0f / 100.0f + 0.5f);
}

void motorControllerCreate(MotorController* controller, IDriveOutput* output) {
  controller->output = output;
  controller->direction = MOTOR_EDASI;
  controller->pendingDirection = MOTOR_EDASI;
  controller->targetPercent = 40;
  controller->appliedPercent = 0.0f;
  controller->running = 0;
  controller->stopping = 0;
  controller->changingDirection = 0;
  controller->braking = 0;
  controller->lastUpdate = 0;
  activeController = controller;
}

IMotor* motorControllerInterface(MotorController* controller) {
  activeController = controller;
  motorInterface.begin = motorControllerBegin;
  motorInterface.update = motorControllerUpdate;
  motorInterface.start = motorControllerStart;
  motorInterface.stop = motorControllerStop;
  motorInterface.setSpeedPercent = motorControllerSetSpeed;
  motorInterface.setDirection = motorControllerSetDirection;
  motorInterface.getStatus = motorControllerStatus;
  return &motorInterface;
}

void motorControllerBegin(void) {
  MotorController* c = activeController;
  c->output->begin();
  c->output->setDirection(c->direction);
  c->output->off();
  c->lastUpdate = micros();
}

void motorControllerUpdate(void) {
  MotorController* c = activeController;
  uint32_t now = micros();
  uint32_t elapsed = now - c->lastUpdate;
  float requested;
  float rate;
  float step;

  if (elapsed < 10000UL) return;
  c->lastUpdate = now;
  rate = (!c->running || c->stopping || c->changingDirection) ? 40.0f : 25.0f;
  step = rate * ((float)elapsed / 1000000.0f);
  requested = (!c->running || c->stopping || c->changingDirection)
                ? 0.0f : (float)c->targetPercent;

  if (c->appliedPercent < requested) {
    c->appliedPercent += step;
    if (c->appliedPercent > requested) c->appliedPercent = requested;
  } else if (c->appliedPercent > requested) {
    c->appliedPercent -= step;
    if (c->appliedPercent < requested) c->appliedPercent = requested;
  }

  if (c->appliedPercent <= 0.0f) {
    c->appliedPercent = 0.0f;
    if (c->changingDirection) {
      c->output->off();
      c->direction = c->pendingDirection;
      c->output->setDirection(c->direction);
      c->changingDirection = 0;
    } else if (c->stopping) {
      c->output->brake();
      c->stopping = 0;
      c->braking = 1;
    } else if (!c->running) {
      c->output->off();
    }
  } else if (c->running && !c->braking) {
    c->output->drive(percentToPwm(c->appliedPercent));
  }
}

void motorControllerStart(void) {
  MotorController* c = activeController;
  if (c->braking) {
    c->output->off();
    c->braking = 0;
  }
  c->stopping = 0;
  c->running = 1;
}

void motorControllerStop(void) {
  MotorController* c = activeController;
  c->running = 0;
  c->stopping = 1;
  c->changingDirection = 0;
}

void motorControllerSetSpeed(uint8_t percent) {
  activeController->targetPercent = percent > 100 ? 100 : percent;
}

void motorControllerSetDirection(MotorDirection direction) {
  MotorController* c = activeController;
  if (direction == c->direction && !c->changingDirection) return;
  if (!c->running && c->appliedPercent <= 0.0f) {
    if (c->braking) {
      c->output->off();
      c->braking = 0;
    }
    c->direction = direction;
    c->pendingDirection = direction;
    c->output->setDirection(direction);
    return;
  }
  c->pendingDirection = direction;
  c->changingDirection = 1;
  c->stopping = 0;
}

MotorStatus motorControllerStatus(void) {
  MotorController* c = activeController;
  MotorStatus status;
  status.direction = c->changingDirection ? c->pendingDirection : c->direction;
  status.targetPercent = c->targetPercent;
  status.appliedPercent = (uint8_t)(c->appliedPercent + 0.5f);
  if (c->braking) status.state = MOTOR_PIDURDATUD;
  else if (c->changingDirection) status.state = MOTOR_SUUNAVAHETUS;
  else if (c->running) status.state = MOTOR_TOIMIB;
  else status.state = MOTOR_SEISATUD;
  return status;
}
