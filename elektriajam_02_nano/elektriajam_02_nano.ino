#include "devices/IMotor.h"
#include "signal/DriveOutput.h"
#include "signal/Vnh5019Output.h"
#include "signal/Drv8825Output.h"
#include "control/MotorController.h"
#include "app/SerialConsole.h"

static DriveOutput dcOutput;
static DriveOutput stepOutput;
static MotorController dcController;
static MotorController stepController;
static IMotor dcMotor;
static IMotor stepMotor;

void setup(void) {
  vnhCreate(&dcOutput);
  drv8825Create(&stepOutput);
  motorControllerCreate(&dcController, &dcOutput, MOTOR_ALALISVOOL);
  motorControllerCreate(&stepController, &stepOutput, MOTOR_SAMM);
  motorControllerInterface(&dcController, &dcMotor);
  motorControllerInterface(&stepController, &stepMotor);
  dcMotor.begin(dcMotor.context);
  stepMotor.begin(stepMotor.context);
  consoleBegin(&dcMotor, &stepMotor, 115200);
}

void loop(void) {
  dcMotor.update(dcMotor.context);
  stepMotor.update(stepMotor.context);
  consoleUpdate();
}
