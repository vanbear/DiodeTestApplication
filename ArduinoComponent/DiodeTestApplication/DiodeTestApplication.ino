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
  pinMode(global::pins::HALL_SENSOR_A, INPUT);
  pinMode(global::pins::HALL_SENSOR_B, INPUT);

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
  motorBase.changeDirection(true);
  
  Serial.println("Calibration start.");
  while ( (digitalRead(global::pins::HALL_SENSOR_A) == HIGH) && (digitalRead(global::pins::HALL_SENSOR_B) == HIGH) )
  {
    motorBase.doNSteps(stepsPerRevolution);
    stepsDone += stepsPerRevolution;
    if (stepsDone >= global::steps::MOTOR_BASE_MAX)
    {
      Serial.println("Calibration failed - Hall sensor didn't activate after full rotation, returning to starting position.");
      motorBase.setSpeed(4);
      motorBase.toggleDirection();
      motorBase.doNSteps(stepsDone);
      return false;
    }
  }
  Serial.println("Calibration succesful.");
  motorBase.doStep(); // last additional step
  delay(2000);
  return true;
}

void getLightDensity()
{
  int valueMain = analogRead(global::pins::LIGHT_SENSOR_MAIN);
  int valueRef = analogRead(global::pins::LIGHT_SENSOR_REF);
  // int valueMap = map(valueMain - valueRef, 0, 1023, 0, 255);
  int valueMap = valueMain - valueRef;

  Serial.print(" Value Main: "); Serial.print(valueMain);
  Serial.print(" | Value Ref: "); Serial.print(valueRef);
  Serial.print(" | Mapped Value: "); Serial.println(valueMap);
}

void measure()
{
  Serial.println("Measure start.");

  if ( digitalRead(global::pins::HALL_SENSOR_A) == LOW )
  {
    Serial.println("Sensor A - direction CCW.");
    motorBase.changeDirection(false);
  }
  else if ( digitalRead(global::pins::HALL_SENSOR_B) == LOW )
  {
    Serial.println("Sensor B - direction CW.");
    motorBase.changeDirection(true);
  }
  else
  {
    Serial.println("Cannot determine base motor direction.");
    return;
  }
  
  int stepsDone_Base = 0;
  int stepsDone_Diode = 0;
  int counterBase = 0;
  int counterDiode = 0;
  motorBase.setStepMode(StepMode::HALF_STEP);

  while (stepsDone_Base < global::steps::MOTOR_BASE_MAX)
  {
    stepsDone_Diode = 0;
    counterDiode = 0;

    while (stepsDone_Diode < global::steps::MOTOR_DIODE_MAX)
    {
      Serial.print(counterBase); Serial.print(" : "); Serial.print(counterDiode);
      getLightDensity();

      motorDiode.doNSteps(global::steps::MOTOR_DIODE_MEASURE_STEP);
      stepsDone_Diode += global::steps::MOTOR_DIODE_MEASURE_STEP;

      counterDiode++;
      delay(global::time::DELAY_BETW_MEASURE_STEPS);
    }

    counterBase++;
    motorDiode.toggleDirection();
    motorBase.doNSteps(global::steps::MOTOR_BASE_MEASURE_STEP);
    stepsDone_Base += global::steps::MOTOR_BASE_MEASURE_STEP;
  }

  getLightDensity(); 
  Serial.println("Measure end.");
}
