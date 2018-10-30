#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
const int stepsPerRevolution2 = 48;  // change this to fit the number of steps per revolution
// for your motor

int hall_din=2;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 11, 9, 10, 8);
Stepper myStepper2(stepsPerRevolution2, 7, 5, 6, 4);

/////////////////////////// MICROSTEPPING
int A = 11;
int B = 9;
int C = 10;
int D = 3;
long duty = 50;
int waitMicroSeconds = 500;
int pulseCount = 5;

void one(){
  digitalWrite(A, HIGH);   
  digitalWrite(B, LOW);   
  digitalWrite(C, HIGH);   
  digitalWrite(D, LOW);   
}

void two(){
  digitalWrite(A, HIGH);   
  digitalWrite(B, LOW);   
  digitalWrite(C, LOW);   
  digitalWrite(D, HIGH);   
}

void three(){
  digitalWrite(A, LOW);   
  digitalWrite(B, HIGH);   
  digitalWrite(C, LOW);   
  digitalWrite(D, HIGH);   
}

void four(){
  digitalWrite(A, LOW);   
  digitalWrite(B, HIGH);   
  digitalWrite(C, HIGH);   
  digitalWrite(D, LOW);   
}

void oneB(){
  digitalWrite(A, HIGH);   
  digitalWrite(B, LOW);   
  digitalWrite(C, LOW);   
  digitalWrite(D, LOW);   
}

void twoB(){
  digitalWrite(A, LOW);   
  digitalWrite(B, LOW);   
  digitalWrite(C, LOW);   
  digitalWrite(D, HIGH);   
}

void threeB(){
  digitalWrite(A, LOW);   
  digitalWrite(B, HIGH);   
  digitalWrite(C, LOW);   
  digitalWrite(D, LOW);   
}

void fourB(){
  digitalWrite(A, LOW);   
  digitalWrite(B, LOW);   
  digitalWrite(C, HIGH);   
  digitalWrite(D, LOW);   
}


// main routine to microstep
void doStep(int st){
  
  long dt1 = waitMicroSeconds * duty / 100;
  long dt2 = waitMicroSeconds * (100-duty) / 100;

  for (int j = 0; j < pulseCount; j++){
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

// disable motor
void motorOff(){
  /* Important note:
       Turning off the motor will make it go into a 'rest' state. 
       When using microsteps (or even full steps), this may not be the last active step. 
       So using this routine may change the position of the motor a bit.
  */
  
  digitalWrite(A, LOW);   
  digitalWrite(B, LOW);   
  digitalWrite(C, LOW);   
  digitalWrite(D, LOW);   
}

// microstepping 16 steps :
void do16Steps(int cnt, boolean forwards){

  const int list[] = {1,21,11,31,2,22,12,32,3,23,13,33,4,24,14,34};
  for (int i = 0; i < cnt; i++){
    duty = 50;
    if (forwards)
      {for (int j = 0; j <= 15; j++){doStep(list[j]);}}
    else
      {for (int j = 15; j >= 0; j--){doStep(list[j]);}}
  }  
}
/////////////////////////////////////////////////////////////////////


float checkLightDensity()
{
  Serial.println("Light density");
}

bool calibrate()
{
    Serial.println("Calibration start.");
    
    //myStepper.setSpeed(50);
    
    while (digitalRead(hall_din)==HIGH)
    {
    //  myStepper.step(1);
      do16Steps(1, true);
    }
    Serial.println("Calibration end.");
    delay(1000);
    motorOff();
    return true;
}

void measure()
{
    myStepper.setSpeed(100);
    myStepper2.setSpeed(150);
    // step one revolution  in one direction:
    int steps1 = 25;
    int steps2 = 32;
    for (int i=0; i<steps1; i++)
    {
      do16Steps(1, true);
      //myStepper.step(stepsPerRevolution/steps1);
      delay(100);
      for (int j=0; j<steps2; j++)
      {
          Serial.println((String)i+":"+(String)j);
          myStepper2.step(2048/steps2);
          delay(100);
          //checkLightDensity();
      }
      
    }
    Serial.println("Koniec.");
}

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
  pinMode(A, OUTPUT);     
  pinMode(B, OUTPUT);     
  pinMode(C, OUTPUT);     
  pinMode(D, OUTPUT);
  if(calibrate())
  {
    measure();
  }
  
}

void loop() {
  

}
