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

  // LED
  pinMode(global::pins::LED, OUTPUT);
  digitalWrite(global::pins::LED, LOW);

  // hall sensor
  pinMode(global::pins::HALL_SENSOR_A, INPUT);
  pinMode(global::pins::HALL_SENSOR_B, INPUT);

  // motors
  motorDiode.setDelay(1);
  motorBase.setDelay(3);

  // program start
  if (calibrate())
  {
    startMeasurement();
  }
  else
  {
    Serial.println("Measure cancelled.");
  }
}

void loop()
{
  // nothing
}

bool calibrate()
{
  int stepsDone = 0;
  const int stepsPerRevolution = 1;
  motorBase.setDelay(8);
  motorBase.changeDirection(StepDirection::COUNTER_CLOCKWISE);
  
  Serial.println("Calibration start.");
  while ( (digitalRead(global::pins::HALL_SENSOR_A) == HIGH) && (digitalRead(global::pins::HALL_SENSOR_B) == HIGH) )
  {
    motorBase.doNSteps(stepsPerRevolution);
    stepsDone += stepsPerRevolution;
    if (stepsDone >= global::steps::MOTOR_BASE_MAX)
    {
      Serial.println("Calibration failed - Hall sensor didn't activate after full rotation, returning to starting position.");
      motorBase.setDelay(8);
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

int getLightDensity()
{
  int valueMain = analogRead(global::pins::LIGHT_SENSOR_MAIN);
  int valueRef = analogRead(global::pins::LIGHT_SENSOR_REF);
  return map(valueMain - valueRef, 0, 1023, 0, 255);
}

void startMeasurement()
{
  Serial.println("Measure start.");
  digitalWrite(global::pins::LED, HIGH);

  if ( digitalRead(global::pins::HALL_SENSOR_A) == LOW )
  {
    Serial.println("Sensor A - direction CW.");
    motorBase.changeDirection(StepDirection::CLOCKWISE);
  }
  else if ( digitalRead(global::pins::HALL_SENSOR_B) == LOW )
  {
    Serial.println("Sensor B - direction CCW.");
    motorBase.changeDirection(StepDirection::COUNTER_CLOCKWISE);
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
      sendData(counterBase, counterDiode, motorBase.getCurrentDirection(), motorDiode.getCurrentDirection(), getLightDensity());

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

  sendData(counterBase, counterDiode, motorBase.getCurrentDirection(), motorDiode.getCurrentDirection(), getLightDensity());

  delay(1000);
  digitalWrite(global::pins::LED, LOW);
  Serial.println("Measure end.");
}

void sendData(int alpha, int beta, StepDirection dirA, StepDirection dirB, int value)
{
  Serial.print(" Send data Alpha: "); Serial.print(alpha);
  Serial.print(" | Beta: "); Serial.print(beta);
  Serial.print(" | dirA: "); Serial.print(static_cast<bool>(dirA));
  Serial.print(" | dirB: "); Serial.print(static_cast<bool>(dirB));
  Serial.print(" | value: "); Serial.println(value);
}
