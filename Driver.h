#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class RhinoMotor {
  public:
    int _DIR_PIN;
    int _PWM_PIN;
    
    RhinoMotor(int DIR_PIN, int PWM_PIN);

    void setSpeed(int speed);
    void stop();
};

#endif 