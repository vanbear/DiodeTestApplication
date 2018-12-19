#include "MyMotor.h"
#include "DataTransferService.h"
#include "GlobalDefs.h"

Pins pins1(2, 3, 4, 5);
MyMotor motorDiode(pins1, 200);

Pins pins2(8, 9, 10, 11);
MyMotor motorBase(pins2, 200);

DataTransferService dataService;
String receivedMessage;
bool isReceivingMessage(false);

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
  motorBase.setDelay(5);
  pinMode(gd::pins::MOTOR_SLEEP, OUTPUT);
}

void loop()
{
  if(readStartMessage())
  {
    if (calibrate())
    {
      startMeasurement();
    }
    sleepMotors();
  }
}

bool readStartMessage()
{
  if (Serial.available() > 0)
  {
    char receivedChar = Serial.read();
    
    if (receivedChar == data::END_MSG)
    {
      isReceivingMessage = false;
      return (receivedMessage == data::START);
    }
    if (isReceivingMessage)
    {
      receivedMessage.concat(receivedChar);
    }
    if (receivedChar == data::BEGIN_MSG)
    {
      isReceivingMessage = true;
      receivedMessage = String();
    }
  }
  return false;
}

bool calibrate()
{
  activateMotors();
  motorBase.setStepMode(StepMode::FULL_STEP);

  int stepsDone = 0;
  motorBase.changeDirection(StepDirection::COUNTER_CLOCKWISE);
  
  dataService.sendMessage(data::CALIBRATION_START, data::TRUE_TEXT_VALUE);
  while ( (digitalRead(gd::pins::HALL_SENSOR_A) == HIGH) && (digitalRead(gd::pins::HALL_SENSOR_B) == HIGH) )
  {
    motorBase.doNSteps(gd::steps::MOTOR_BASE_MEASURE_STEP);
    stepsDone += gd::steps::MOTOR_BASE_MEASURE_STEP;
    if (stepsDone >= gd::steps::MOTOR_BASE_MAX)
    {
      dataService.sendError("Calibration failed - Hall sensor didn't activate after full rotation, returning to starting position.");
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
      if (readStartMessage())
      {
        endMeasurement();
        dataService.sendError("Measurement ended by user.");
        return;
      }
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

  delay(2000);
  endMeasurement();
}

void endMeasurement()
{
  digitalWrite(gd::pins::LED, LOW);
}