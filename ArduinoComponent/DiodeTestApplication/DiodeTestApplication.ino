#include "MyMotor.h"

Pins pins1(2, 3, 4, 5);
MyMotor motor1(pins1, 200);

Pins pins2(8, 9, 10, 11);
MyMotor motor2(pins2, 200);

void setup()
{
  // serial
  Serial.begin(9600);

  // motors
  motor1.setSpeed(1);
  motor2.setStepMode(StepMode::FULL_STEP);
  motor2.setSpeed(3);
  motor2.setStepMode(StepMode::FULL_STEP);
}

void loop() 
{
  motor2.doNSteps(200);
  motor2.toggleDirection();
  delay(800);

  motor1.doNSteps(500);
  motor1.toggleDirection();
  delay(800);
}
