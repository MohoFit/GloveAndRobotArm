#include <Servo.h>
#include <Wire.h>
#include <MPU6050.h>
#include <I2Cdev.h>
#include <inttypes.h>


//naming servo motors
Servo Spin;
Servo Up;
Servo Forward;
Servo Claw;


//Here individual I2C addresses can be supplied as inputs.

MPU6050 mpu;
 //define output variables from accelerometer (0x69)
int16_t ax, ay, az;
int16_t gx, gy, gz;


//define output that will set the degrees upon startup
float SpinDegree = 80; // dead zone: left 20 - 120 right
float ForwardDegree = 100; // deadzone: forward 55 - 120 backward
float UpDegree = 80; // deadzone: up 100 - 72 down
float ClawDegree = 0; // deadzone: closed 80 -  20 open

//attributes for accelerometer to work with
int x;
int y;
int z;
//for claw button
int buttonPin = 0;
int readValue;



//define variables of increments
float Clawinc = 0.1;
float increment;
float spincrement;
float uncrement;


void setup() {
  Wire.begin(); //I2C master mode for mpu
  mpu.initialize();

  //claw button
  pinMode(buttonPin, INPUT_PULLUP);


  //assigning servos to pins
  Spin.attach(2);
  Claw.attach(11);
  Up.attach(4);
  Forward.attach(3);

  //initial position
  Claw.write(ClawDegree);
  Forward.write(ForwardDegree);
  Spin.write(SpinDegree);
  Up.write(UpDegree);

  //setting the baud rate for serial data communication
  Serial.begin(9600);


}

void loop() {


  //mpu.getMotion6(&AcX, &AcY, &AcZ, &Tmp, &GyX, &GyY, &GyZ);
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);


  increment = 1;
  uncrement = 1;

  //maping mpu readings to a new range
  x = map(ax, -17000, 17000, 0, 255 ); //up down movement of acc
  y = map(ay, -17000, 17000, 0, 255 ); // left right
  z = map(az, -17000, 17000, 0, 255 );



  //for plotting axis degree and miliseconds inorder to work with mpu readings and input results
  long int time = millis(); //variable for seconds
  
  Serial.print("Time (ms) = ");
  //Serial.print(time);
  Serial.print(" Axis X = ");
  Serial.print(x);
  Serial.print("  ");
  Serial.print("Axis Y = ");
  Serial.println(y);
  Serial.print("  ");
  //Serial.print("Axis Z  = ");
  //Serial.println(z);
  //Serial.println(ax);

  
  //Spin section, from y
  if(y < 50){
    spincrement = y/150;
    SpinDegree = SpinDegree + 1;
    if(SpinDegree >= 140) {
      SpinDegree = 140;
    }
  }
  else{
    if(y > 220)
    spincrement = y/150;
    SpinDegree = SpinDegree - spincrement;
    if(SpinDegree <= 20) {
      SpinDegree = 20;}
  }
  Spin.write(SpinDegree);
  //end of spin


   //Forward section, from Y in acc
   if(ax > 10000 && ForwardDegree >= 60){  //forward
    ForwardDegree = ForwardDegree - increment;
   }
   else{
    if(ax < -14000 && ForwardDegree <= 120) //backward
    ForwardDegree = ForwardDegree + increment;
   }
   Forward.write(ForwardDegree);
   //end of forward section


     //up and down section, from Z in acc
   if(ax <= -10000 && UpDegree < 100 ){ //up
    UpDegree = UpDegree + uncrement;
   }  
   else{
    if(ax >= 14000 && UpDegree > 75 && ForwardDegree < 60 ) //down
      UpDegree = UpDegree - uncrement;
   }
   Up.write(UpDegree);
   //end of up/down section

  //button code for claw
    readValue = digitalRead(buttonPin);
    if (readValue == 1){
      Claw.write(0);
      Serial.print("Claw Open");
    } else{
      Claw.write(80);
      Serial.print("Claw Closed");
    }
    //end of Claw section

    delay(300);





}
