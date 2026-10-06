#include "mcu/NanoPins.h"
#include "devices/IMotor.h"
#include "signal/IDriveOutput.h"
#include "signal/Vnh5019Output.h"
#include "control/MotorController.h"
#include "app/SerialConsole.h"

static IDriveOutput driveOutput;
static MotorController controller;
static IMotor* motor;

void setup(void) {
  vnhCreate(&driveOutput);
  motorControllerCreate(&controller, &driveOutput);
  motor = motorControllerInterface(&controller);
  motor->begin();
  consoleBegin(motor, 115200);
}

void loop(void) {
  motor->update();
  consoleUpdate();
}
