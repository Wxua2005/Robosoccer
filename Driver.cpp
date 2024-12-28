#include "Driver.h"

RhinoMotor::RhinoMotor(int DIR_PIN, int PWM_PIN) : _DIR_PIN(DIR_PIN), _PWM_PIN(PWM_PIN) {
  pinMode(_DIR_PIN, OUTPUT);
  pinMode(_PWM_PIN, OUTPUT);
}

void RhinoMotor::setSpeed(int speed) {
  if (speed >= 0) {
    // Forward
    digitalWrite(_DIR_PIN, HIGH);
    analogWrite(_PWM_PIN, speed);
  }
  else {
    // Backward
    digitalWrite(_DIR_PIN, LOW);
    analogWrite(_PWM_PIN, -speed);
  }
}

void RhinoMotor::stop() {
  analogWrite(_PWM_PIN, 0);
}

