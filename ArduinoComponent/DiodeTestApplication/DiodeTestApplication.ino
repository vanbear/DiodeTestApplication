#include "Motor/MotorDriver.h"

float checkLightDensity()
{
  Serial.println("Light density");
}

bool calibrate()
{
    Serial.println("Calibration start.");
    while (digitalRead(motor::HALL_IN_PIN)==HIGH)
    {
      motor::do16Steps(1, true);
    }
    Serial.println("Calibration end.");
    delay(1000);
    motor::motorOff();
    return true;
}

void measure()
{
    motor::myStepper2.setSpeed(150);
    int steps1 = 25;
    int steps2 = 32;
    for (int i=0; i<steps1; i++)
    {
      motor::do16Steps(1, true);
      delay(100);
      for (int j=0; j<steps2; j++)
      {
          Serial.println((String)i+":"+(String)j);
          motor::myStepper2.step(2048/steps2);
          delay(100);
          //checkLightDensity();
      }
    }
    motor::motorOff();
    Serial.println("Koniec.");
}

void initPinModes()
{
pinMode(motor::MOTOR1_PIN_A, OUTPUT);     
pinMode(motor::MOTOR1_PIN_B, OUTPUT);     
pinMode(motor::MOTOR1_PIN_C, OUTPUT);     
pinMode(motor::MOTOR1_PIN_D, OUTPUT);

pinMode(motor::MOTOR2_PIN_A, OUTPUT);     
pinMode(motor::MOTOR2_PIN_B, OUTPUT);     
pinMode(motor::MOTOR2_PIN_C, OUTPUT);     
pinMode(motor::MOTOR2_PIN_D, OUTPUT);

pinMode(motor::HALL_IN_PIN, INPUT);
}

void setup()
{
  Serial.begin(9600);
  initPinModes();
  if(calibrate())
  {
    measure();
  }
}

void loop() {
// done in setup
}
