#include "SerialConsole.h"

static IMotor* motors[2];
static IMotor* selected;
static char buffer[48];
static uint8_t length;
static uint8_t overflow;

static uint8_t sameText(const char* a, const char* b) {
  while (*a && *b && *a == *b) { a++; b++; }
  return *a == '\0' && *b == '\0';
}

static uint8_t startsText(const char* text, const char* start) {
  while (*start) {
    if (*text++ != *start++) return 0;
  }
  return 1;
}

static uint8_t readPercent(const char* text, uint8_t* result) {
  uint16_t value = 0;
  uint8_t digits = 0;
  if (*text == '\0') return 0;
  while (*text) {
    if (*text < '0' || *text > '9' || digits == 3) return 0;
    value = value * 10 + (uint16_t)(*text++ - '0');
    digits++;
  }
  if (value > 100) return 0;
  *result = (uint8_t)value;
  return 1;
}

static const __FlashStringHelper* name(const IMotor* motor) {
  return motor->kind == MOTOR_ALALISVOOL ? F("DC") : F("SAMM");
}

static const __FlashStringHelper* stateName(MotorState state) {
  if (state == MOTOR_SEISATUD) return F("seisatud");
  if (state == MOTOR_TOIMIB) return F("toimib");
  if (state == MOTOR_SUUNAVAHETUS) return F("suunavahetus");
  return F("pidurdatud");
}

static void printOne(IMotor* motor) {
  MotorStatus status = motor->getStatus(motor->context);
  Serial.print(name(motor));
  Serial.print(F(": olek=")); Serial.print(stateName(status.state));
  Serial.print(F(", suund="));
  Serial.print(status.direction == MOTOR_EDASI ? F("EDASI") : F("TAGASI"));
  Serial.print(F(", soov=")); Serial.print(status.targetPercent);
  Serial.print(F("%, rakendatav=")); Serial.print(status.appliedPercent);
  Serial.println(F("%"));
}

static void printStatus(void) {
  Serial.println(F("OLEK:"));
  printOne(motors[0]);
  printOne(motors[1]);
  Serial.print(F("Kaskude sihtmärk: ")); Serial.println(name(selected));
}

static void printHelp(void) {
  Serial.println(F("Käsud: MOOTOR DC | MOOTOR SAMM"));
  Serial.println(F("        KAIVITA | SEISKA | SUUND EDASI | SUUND TAGASI"));
  Serial.println(F("        KIIRUS 0..100 | OLEK | ABI"));
}

static void processCommand(const char* command) {
  uint8_t speed;
  if (sameText(command, "MOOTOR DC")) {
    selected = motors[0]; Serial.println(F("Käskude sihtmärk: DC."));
  } else if (sameText(command, "MOOTOR SAMM")) {
    selected = motors[1]; Serial.println(F("Käskude sihtmärk: SAMM."));
  } else if (sameText(command, "KAIVITA")) {
    selected->start(selected->context); printStatus();
  } else if (sameText(command, "SEISKA")) {
    selected->stop(selected->context); printStatus();
  } else if (sameText(command, "SUUND EDASI")) {
    selected->setDirection(selected->context, MOTOR_EDASI); printStatus();
  } else if (sameText(command, "SUUND TAGASI")) {
    selected->setDirection(selected->context, MOTOR_TAGASI); printStatus();
  } else if (startsText(command, "KIIRUS ")) {
    if (!readPercent(command + 7, &speed))
      Serial.println(F("VIGA: KIIRUS peab olema taisarv vahemikus 0..100."));
    else { selected->setSpeedPercent(selected->context, speed); printStatus(); }
  } else if (sameText(command, "OLEK")) printStatus();
  else if (sameText(command, "ABI")) printHelp();
  else if (command[0] != '\0')
    Serial.println(F("VIGA: tundmatu voi vigane kasuk. Kasuta ABI."));
}

void consoleBegin(IMotor* dc, IMotor* stepper, unsigned long baud) {
  motors[0] = dc; motors[1] = stepper; selected = dc;
  length = 0; overflow = 0; Serial.begin(baud); printHelp();
}

void consoleUpdate(void) {
  while (Serial.available() > 0) {
    char character = (char)Serial.read();
    if (character == '\r') continue;
    if (character == '\n') {
      if (overflow) Serial.println(F("VIGA: kasuk on liiga pikk."));
      else { buffer[length] = '\0'; processCommand(buffer); }
      length = 0; overflow = 0;
    } else if (length < sizeof(buffer) - 1) buffer[length++] = character;
    else overflow = 1;
  }
}
