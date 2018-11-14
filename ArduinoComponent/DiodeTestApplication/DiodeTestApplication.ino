#include "MyMotor.h"
#include "GlobalDefs.h"

Pins pins1(2, 3, 4, 5);
MyMotor motorDiode(pins1, 200);

Pins pins2(8, 9, 10, 11);
MyMotor motorBase(pins2, 200);

void setup()
{
  // serial
  Serial.begin(9600);

  // hall sensor
  pinMode(global::pins::HALL_SENSOR, INPUT);

  // motors
  motorDiode.setSpeed(1);
  motorBase.setSpeed(3);

  // program start
  if (calibrate())
  {
    measure();
  }
  else
  {
    Serial.println("Measure cancelled.");
  }
}

void loop() 
{
}

bool calibrate()
{
  int stepsDone = 0;
  const int stepsPerRevolution = 1;
  motorBase.setSpeed(6);
  
  Serial.println("Calibration start.");
  while (digitalRead(global::pins::HALL_SENSOR) == HIGH)
  {
    motorBase.doNSteps(stepsPerRevolution);
    stepsDone += stepsPerRevolution;
    if (stepsDone >= global::steps::MOTOR_BASE_MAX)
    {
      Serial.println("Calibration failed - Hall sensor didn't activate after full rotation, returning to starting position.");
      motorBase.setSpeed(1);
      motorBase.toggleDirection();
      motorBase.doNSteps(stepsDone);
      return false;
    }
  }
  Serial.println("Calibration succesful.");
  delay(2000);
  return true;
}

void measure()
{
  Serial.println("Measure start.");

  int stepsDone_Base = 0;
  int stepsDone_Diode = 0;

  while (stepsDone_Base < global::steps::MOTOR_BASE_MAX)
  {
    Serial.println("Base step");
    stepsDone_Diode = 0;

    motorBase.doNSteps(global::steps::MOTOR_BASE_MEASURE_STEP);
    stepsDone_Base += global::steps::MOTOR_BASE_MEASURE_STEP;
    while (stepsDone_Diode < global::steps::MOTOR_DIODE_MAX)
    {
      Serial.println("Diode step");
      motorDiode.doNSteps(global::steps::MOTOR_DIODE_MEASURE_STEP);
      stepsDone_Diode += global::steps::MOTOR_DIODE_MEASURE_STEP;

      delay(global::time::DELAY_BETW_MEASURE_STEPS);
    }
  }

  Serial.println("Measure end.");
}
