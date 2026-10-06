#include "SerialConsole.h"

static IMotor* motor;
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
    value = value * 10 + (uint16_t)(*text - '0');
    digits++;
    text++;
  }
  if (value > 100) return 0;
  *result = (uint8_t)value;
  return 1;
}

static void printStatus(void) {
  MotorStatus status = motor->getStatus();
  Serial.print(F("OLEK: "));
  if (status.state == MOTOR_SEISATUD) Serial.print(F("seisatud"));
  else if (status.state == MOTOR_TOIMIB) Serial.print(F("toimib"));
  else if (status.state == MOTOR_SUUNAVAHETUS) Serial.print(F("suunavahetus"));
  else Serial.print(F("pidurdatud"));
  Serial.print(F(", suund="));
  Serial.print(status.direction == MOTOR_EDASI ? F("EDASI") : F("TAGASI"));
  Serial.print(F(", kiiruseade="));
  Serial.print(status.targetPercent);
  Serial.print(F("%, rakendatud="));
  Serial.print(status.appliedPercent);
  Serial.println(F("% (PWM-seade, mitte RPM)"));
}

static void printHelp(void) {
  Serial.println(F("Käsud: KAIVITA | SEISKA | SUUND EDASI | SUUND TAGASI"));
  Serial.println(F("        KIIRUS 0..100 | OLEK | ABI"));
}

static void processCommand(const char* command) {
  uint8_t speed;
  if (sameText(command, "KAIVITA")) {
    motor->start(); printStatus();
  } else if (sameText(command, "SEISKA")) {
    motor->stop(); printStatus();
  } else if (sameText(command, "SUUND EDASI")) {
    motor->setDirection(MOTOR_EDASI); printStatus();
  } else if (sameText(command, "SUUND TAGASI")) {
    motor->setDirection(MOTOR_TAGASI); printStatus();
  } else if (startsText(command, "KIIRUS ")) {
    if (!readPercent(command + 7, &speed)) {
      Serial.println(F("VIGA: KIIRUS peab olema taisarv vahemikus 0..100."));
    } else {
      motor->setSpeedPercent(speed); printStatus();
    }
  } else if (sameText(command, "OLEK")) {
    printStatus();
  } else if (sameText(command, "ABI")) {
    printHelp();
  } else if (command[0] != '\0') {
    Serial.println(F("VIGA: tundmatu voi vigane kasuk. Kasuta ABI."));
  }
}

void consoleBegin(IMotor* motorInterface, unsigned long baud) {
  motor = motorInterface;
  length = 0;
  overflow = 0;
  Serial.begin(baud);
  printHelp();
}

void consoleUpdate(void) {
  while (Serial.available() > 0) {
    char character = (char)Serial.read();
    if (character == '\r') continue;
    if (character == '\n') {
      if (overflow) Serial.println(F("VIGA: kasuk on liiga pikk."));
      else {
        buffer[length] = '\0';
        processCommand(buffer);
      }
      length = 0;
      overflow = 0;
    } else if (length < sizeof(buffer) - 1) {
      buffer[length++] = character;
    } else {
      overflow = 1;
    }
  }
}
