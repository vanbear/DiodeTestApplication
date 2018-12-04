#include "MyMotor.h"
#include "DataTransferService.h"
#include "GlobalDefs.h"

Pins pins1(2, 3, 4, 5);
MyMotor motorDiode(pins1, 200);

Pins pins2(8, 9, 10, 11);
MyMotor motorBase(pins2, 200);

DataTransferService dataService;

void setup()
{
  Serial.begin(9600);

  // LED
  pinMode(gd::pins::LED, OUTPUT);
  digitalWrite(gd::pins::LED, LOW);

  // hall sensor
  pinMode(gd::pins::HALL_SENSOR_A, INPUT);
  pinMode(gd::pins::HALL_SENSOR_B, INPUT);

  // motors
  motorDiode.setDelay(1);
  motorBase.setDelay(3);
  pinMode(gd::pins::MOTOR_SLEEP, OUTPUT);

  // program start
  if (calibrate())
  {
    startMeasurement();
  }
  else
  {
    sleepMotors();
  }
  sleepMotors();
}

void loop()
{
  // nothing
}

bool calibrate()
{
  activateMotors();

  int stepsDone = 0;
  const int stepsPerRevolution = 1;
  motorBase.setDelay(8);
  motorBase.changeDirection(StepDirection::COUNTER_CLOCKWISE);
  
  dataService.sendMessage(data::CALIBRATION_START, data::TRUE_TEXT_VALUE);
  while ( (digitalRead(gd::pins::HALL_SENSOR_A) == HIGH) && (digitalRead(gd::pins::HALL_SENSOR_B) == HIGH) )
  {
    motorBase.doNSteps(stepsPerRevolution);
    stepsDone += stepsPerRevolution;
    if (stepsDone >= gd::steps::MOTOR_BASE_MAX)
    {
      dataService.sendError("Calibration failed - Hall sensor didn't activate after full rotation, returning to starting position.");
      motorBase.setDelay(8);
      motorBase.toggleDirection();
      motorBase.doNSteps(stepsDone);
      return false;
    }
  }
  motorBase.doStep(); // last additional step
  return true;
}

int getLightDensity()
{
  int valueMain = analogRead(gd::pins::LIGHT_SENSOR_MAIN);
  int valueRef = analogRead(gd::pins::LIGHT_SENSOR_REF);
  return map(valueMain - valueRef, 0, 1023, 0, 255);
}

void startMeasurement()
{
  activateMotors();
  digitalWrite(gd::pins::LED, HIGH);
  String initMessage =  data::SEMI_COLON + String(gd::steps::MOTOR_BASE_MAX/gd::steps::MOTOR_BASE_MEASURE_STEP) + data::SEMI_COLON
    + String(gd::steps::MOTOR_DIODE_MAX/gd::steps::MOTOR_DIODE_MEASURE_STEP);
  
  // determine starting point
  if ( digitalRead(gd::pins::HALL_SENSOR_A) == LOW )
  {
    dataService.sendMessage(data::MEASURE_START, data::HALL_A + initMessage);
    motorBase.changeDirection(StepDirection::CLOCKWISE);
  }
  else if ( digitalRead(gd::pins::HALL_SENSOR_B) == LOW )
  {
    dataService.sendMessage(data::MEASURE_START, data::HALL_B + initMessage);
    motorBase.changeDirection(StepDirection::COUNTER_CLOCKWISE);
  }
  else
  {
    dataService.sendError("Cannot determine base motor direction.");
    return;
  }

  delay(2000);
  
  // start measurement
  int stepsDone_Base = 0;
  int stepsDone_Diode = 0;
  int counterBase = 0;
  int counterDiode = 0;
  motorBase.setStepMode(StepMode::HALF_STEP);

  while (stepsDone_Base < gd::steps::MOTOR_BASE_MAX)
  {
    stepsDone_Diode = 0;
    counterDiode = 0;

    while (stepsDone_Diode < gd::steps::MOTOR_DIODE_MAX)
    {
      dataService.sendData(counterBase, counterDiode, motorBase.getCurrentDirection(), motorDiode.getCurrentDirection(), getLightDensity());

      motorDiode.doNSteps(gd::steps::MOTOR_DIODE_MEASURE_STEP);
      stepsDone_Diode += gd::steps::MOTOR_DIODE_MEASURE_STEP;

      counterDiode++;
      delay(gd::time::DELAY_BETWEEN_MEASURE_STEPS);
    }

    counterBase++;
    motorDiode.toggleDirection();
    motorBase.doNSteps(gd::steps::MOTOR_BASE_MEASURE_STEP);
    stepsDone_Base += gd::steps::MOTOR_BASE_MEASURE_STEP;
  }

  dataService.sendMessage(data::MEASURE_END, data::TRUE_TEXT_VALUE);
  motorDiode.doNSteps(gd::steps::MOTOR_DIODE_MAX);
  motorBase.toggleDirection();
  motorBase.doNSteps(gd::steps::MOTOR_BASE_MAX/2);

  delay(2000);
  digitalWrite(gd::pins::LED, LOW);
}