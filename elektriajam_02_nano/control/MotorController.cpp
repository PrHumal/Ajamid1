#include "MotorController.h"

static uint8_t percentToPwm(float percent) {
  return (uint8_t)(percent * 255.0f / 100.0f + 0.5f);
}

static uint16_t stepInterval(uint8_t percent) {
  return (uint16_t)(3800UL + (uint32_t)(100 - percent) * 200UL);
}

static void controllerBegin(void* context) {
  MotorController* c = (MotorController*)context;
  c->output->begin(c->output->context);
  c->output->setDirection(c->output->context, c->direction);
  c->output->off(c->output->context);
  c->lastUpdate = micros();
  c->lastStep = c->lastUpdate;
}

static void controllerUpdate(void* context) {
  MotorController* c = (MotorController*)context;
  uint32_t now = micros();
  uint32_t elapsed = now - c->lastUpdate;
  float requested;
  float rate;
  float change;
  c->output->service(c->output->context);
  if (elapsed < 10000UL) return;
  c->lastUpdate = now;
  rate = (!c->running || c->stopping || c->changingDirection) ? 40.0f : 25.0f;
  change = rate * ((float)elapsed / 1000000.0f);
  requested = (!c->running || c->stopping || c->changingDirection)
                ? 0.0f : (float)c->targetPercent;
  if (c->appliedPercent < requested) {
    c->appliedPercent += change;
    if (c->appliedPercent > requested) c->appliedPercent = requested;
  } else if (c->appliedPercent > requested) {
    c->appliedPercent -= change;
    if (c->appliedPercent < requested) c->appliedPercent = requested;
  }
  if (c->appliedPercent <= 0.0f) {
    c->appliedPercent = 0.0f;
    if (c->changingDirection) {
      c->output->off(c->output->context);
      c->direction = c->pendingDirection;
      c->output->setDirection(c->output->context, c->direction);
      c->changingDirection = 0;
    } else if (c->stopping) {
      c->stopping = 0;
      if (c->kind == MOTOR_SAMM) c->output->off(c->output->context);
      else {
        c->output->brake(c->output->context);
        c->braking = 1;
      }
    } else if (!c->running) {
      c->output->off(c->output->context);
    }
  } else if (c->running && !c->braking) {
    c->output->drive(c->output->context, percentToPwm(c->appliedPercent));
  }
  if (c->kind == MOTOR_SAMM && c->running && !c->stopping &&
      !c->changingDirection && c->appliedPercent > 0.0f) {
    uint16_t interval = stepInterval((uint8_t)(c->appliedPercent + 0.5f));
    if ((uint32_t)(now - c->lastStep) >= interval) {
      c->lastStep = now;
      c->output->step(c->output->context);
    }
  }
}

static void controllerStart(void* context) {
  MotorController* c = (MotorController*)context;
  if (c->braking) {
    c->output->off(c->output->context);
    c->braking = 0;
  }
  c->stopping = 0;
  c->running = 1;
  if (c->kind == MOTOR_SAMM) c->output->drive(c->output->context, 0);
}

static void controllerStop(void* context) {
  MotorController* c = (MotorController*)context;
  c->running = 0;
  c->stopping = 1;
  c->changingDirection = 0;
}

static void controllerSetSpeed(void* context, uint8_t percent) {
  MotorController* c = (MotorController*)context;
  c->targetPercent = percent > 100 ? 100 : percent;
}

static void controllerSetDirection(void* context, MotorDirection direction) {
  MotorController* c = (MotorController*)context;
  if (direction == c->direction && !c->changingDirection) return;
  if (!c->running && c->appliedPercent <= 0.0f) {
    c->direction = direction;
    c->pendingDirection = direction;
    c->output->setDirection(c->output->context, direction);
    return;
  }
  c->pendingDirection = direction;
  c->changingDirection = 1;
  c->stopping = 0;
}

static MotorStatus controllerStatus(void* context) {
  MotorController* c = (MotorController*)context;
  MotorStatus result;
  result.direction = c->changingDirection ? c->pendingDirection : c->direction;
  result.targetPercent = c->targetPercent;
  result.appliedPercent = (uint8_t)(c->appliedPercent + 0.5f);
  if (c->braking) result.state = MOTOR_PIDURDATUD;
  else if (c->changingDirection) result.state = MOTOR_SUUNAVAHETUS;
  else if (c->running) result.state = MOTOR_TOIMIB;
  else result.state = MOTOR_SEISATUD;
  return result;
}

void motorControllerCreate(MotorController* controller, DriveOutput* output,
                            MotorKind kind) {
  controller->output = output;
  controller->kind = kind;
  controller->direction = MOTOR_EDASI;
  controller->pendingDirection = MOTOR_EDASI;
  controller->targetPercent = 40;
  controller->appliedPercent = 0.0f;
  controller->running = 0;
  controller->stopping = 0;
  controller->changingDirection = 0;
  controller->braking = 0;
  controller->lastUpdate = 0;
  controller->lastStep = 0;
}

void motorControllerInterface(MotorController* controller, IMotor* motor) {
  motor->context = controller;
  motor->kind = controller->kind;
  motor->begin = controllerBegin;
  motor->update = controllerUpdate;
  motor->start = controllerStart;
  motor->stop = controllerStop;
  motor->setSpeedPercent = controllerSetSpeed;
  motor->setDirection = controllerSetDirection;
  motor->getStatus = controllerStatus;
}
