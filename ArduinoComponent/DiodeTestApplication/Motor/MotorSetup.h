#include <Stepper.h>
#include "MotorDefinitions.h"
namespace motor
{

// Stepper myStepper(MOTOR1_RESOLUTION, MOTOR1_PIN_A, MOTOR1_PIN_B, MOTOR1_PIN_C, MOTOR1_PIN_D);
Stepper myStepper2(MOTOR2_RESOLUTION, MOTOR2_PIN_A, MOTOR2_PIN_B, MOTOR2_PIN_C, MOTOR2_PIN_D);

void one(){
  digitalWrite(MOTOR1_PIN_A, HIGH);   
  digitalWrite(MOTOR1_PIN_B, LOW);   
  digitalWrite(MOTOR1_PIN_C, HIGH);   
  digitalWrite(MOTOR1_PIN_D, LOW);   
}

void two(){
  digitalWrite(MOTOR1_PIN_A, HIGH);   
  digitalWrite(MOTOR1_PIN_B, LOW);   
  digitalWrite(MOTOR1_PIN_C, LOW);   
  digitalWrite(MOTOR1_PIN_D, HIGH);   
}

void three(){
  digitalWrite(MOTOR1_PIN_A, LOW);   
  digitalWrite(MOTOR1_PIN_B, HIGH);   
  digitalWrite(MOTOR1_PIN_C, LOW);   
  digitalWrite(MOTOR1_PIN_D, HIGH);   
}

void four(){
  digitalWrite(MOTOR1_PIN_A, LOW);   
  digitalWrite(MOTOR1_PIN_B, HIGH);   
  digitalWrite(MOTOR1_PIN_C, HIGH);   
  digitalWrite(MOTOR1_PIN_D, LOW);   
}

void oneB(){
  digitalWrite(MOTOR1_PIN_A, HIGH);   
  digitalWrite(MOTOR1_PIN_B, LOW);   
  digitalWrite(MOTOR1_PIN_C, LOW);   
  digitalWrite(MOTOR1_PIN_D, LOW);   
}

void twoB(){
  digitalWrite(MOTOR1_PIN_A, LOW);   
  digitalWrite(MOTOR1_PIN_B, LOW);   
  digitalWrite(MOTOR1_PIN_C, LOW);   
  digitalWrite(MOTOR1_PIN_D, HIGH);   
}

void threeB(){
  digitalWrite(MOTOR1_PIN_A, LOW);   
  digitalWrite(MOTOR1_PIN_B, HIGH);   
  digitalWrite(MOTOR1_PIN_C, LOW);   
  digitalWrite(MOTOR1_PIN_D, LOW);   
}

void fourB(){
  digitalWrite(MOTOR1_PIN_A, LOW);   
  digitalWrite(MOTOR1_PIN_B, LOW);   
  digitalWrite(MOTOR1_PIN_C, HIGH);   
  digitalWrite(MOTOR1_PIN_D, LOW);   
}

} // namespace motor