#include "MotorSetup.h"

namespace motor
{

long duty = 50;
void doStep(int st)
{
  int pulseCount = 5;
  int waitMicroSeconds = 500;

  long dt1 = waitMicroSeconds * duty / 100;
  long dt2 = waitMicroSeconds * (100-duty) / 100;

  for (int j = 0; j < pulseCount; j++)
  {
    switch (st){
    case 1: one();break;
    case 2: two();break;
    case 3: three();break;
    case 4: four();break;
    case 11: oneB();break;
    case 12: twoB();break;
    case 13: threeB();break;
    case 14: fourB();break;

    case 21: one();break;
    case 22: two();break;
    case 23: three();break;
    case 24: four();break;
    case 31: oneB();break;
    case 32: twoB();break;
    case 33: threeB();break;
    case 34: fourB();break;
    }

    delayMicroseconds(dt1);

    switch (st){
    case 1: one();break;
    case 2: two();break;
    case 3: three();break;
    case 4: four();break;
    case 11: oneB();break;
    case 12: twoB();break;
    case 13: threeB();break;
    case 14: fourB();break;

    case 21: oneB();break;
    case 22: twoB();break;
    case 23: threeB();break;
    case 24: fourB();break;
    case 31: two();break;
    case 32: three();break;
    case 33: four();break;
    case 34: one();break;
    }
    delayMicroseconds(dt2);
  }
}

void motorOff()
{
  digitalWrite(MOTOR1_PIN_A, LOW);   
  digitalWrite(MOTOR1_PIN_B, LOW);   
  digitalWrite(MOTOR1_PIN_C, LOW);   
  digitalWrite(MOTOR1_PIN_D, LOW);   
}

void do16Steps(int cnt, boolean forwards)
{
  const int list[] = {1,21,11,31,2,22,12,32,3,23,13,33,4,24,14,34};
  for (int i = 0; i < cnt; i++){
    duty = 50;
    if (forwards)
      {for (int j = 0; j <= 15; j++){doStep(list[j]);}}
    else
      {for (int j = 15; j >= 0; j--){doStep(list[j]);}}
  }  
}

} // namespace motor


