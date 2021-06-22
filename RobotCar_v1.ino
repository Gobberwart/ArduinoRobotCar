#include <Servo.h>
//L298N motor driver pins & values
int pinENA=3; // pwm pin for motor A (right wheel)
int pinENB=11; // pwm pin for motor B (left wheel)
int pinLB=7; // define pin6 as left back connect with IN1
int pinLF=6; // define pin9 as left forward connect with IN2
int pinRB=5; // define pin10 as right back connect with IN3
int pinRF=4; // define pin11 as right forward connect with IN4
int motorSpeedA=128; // speed of motor A (right wheel)
int motorSpeedB=128; // speed of motor B (left wheel)

// Servo pins & values
int pinServo=12;
Servo myservo; // new myservo

// Ultrasonic sensor pins & values
int pinEcho=A1; // define ultrasonic receive pin (Echo)
int pinTrig=A0; // define ultrasonic send pin(Trig)
int distanceF=0; // forward distance
int distanceR=0; // right distance
int distanceL=0; // left distance
int delay_time = 250; // set stable time

// Speed and direction values
int valDirection=0; //
int Fgo = 8; // forward
int Rgo = 6; // turn right
int Lgo = 4; // turn left
int Bgo = 2; // backward

void setup() {
  Serial.begin(9600);
  pinMode(pinENA,OUTPUT);
  pinMode(pinENB,OUTPUT);
  pinMode(pinLB,OUTPUT);
  pinMode(pinLF,OUTPUT);
  pinMode(pinRB,OUTPUT);
  pinMode(pinRF,OUTPUT);
  pinMode(pinEcho,INPUT);
  pinMode(pinTrig,OUTPUT);
  pinMode(pinServo,OUTPUT);
  myservo.attach(pinServo); // define the servo pin(PWM)
  analogWrite(pinENA,motorSpeedA);
  analogWrite(pinENB,motorSpeedB);
}

void detection() //test the distance of different direction
{
  ask_pin_F(); // read forward distance
  if(distanceF < 10) // if forward distance less then 10, back up
  {
    Serial.println(" GO BACK ");
    stopp(100);
    retreat(600);
  }
  if(distanceF < 25) // if forward distance less than 25, stop and scan
  {
    stopp(100);
    ask_pin_L(); // scan left distance
    delay(250);
    ask_pin_R(); // scan right distance
    delay(250);
    
    if(distanceL > distanceR) //if left distance more than right distance, turn left
    {
      Serial.println(" GO LEFT ");
      valDirection = Lgo; // ???
    }
    else if(distanceR >= distanceL)//if right distance more than left distance, turn right
    {
      Serial.println(" GO RIGHT ");
      valDirection = Rgo;
    }
    else if (distanceL < 10 && distanceR < 10) //if left distance and right distance both less than 10, go back
    {
      Serial.println(" GO BACK ");
      valDirection = Bgo;
    }
  }
  else
  {
    valDirection = Fgo; // forward go
  }
}

float get_distance(int valPos) {
  myservo.write(valPos);
  delay(300); // Required to give the servo time to get into position before trying to measure
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  float rawDistance = pulseIn(pinEcho, HIGH);
  // Convert ping time in microseconds into distance in cm
  // 343 m/s = 0.0343 cm/ms (343 * 100 / 1000000)
  // Ping time is for return trip so needs to be halved
  float calcDistance = rawDistance * 0.0343 / 2;
  return(calcDistance);
}

void ask_pin_F() // test forward distance
{
  distanceF = get_distance(90);
  Serial.print("F distance:");
  Serial.println(distanceF);
}

void ask_pin_L() // test left distance
{
  distanceL = get_distance(170);
  Serial.print("L distance:");
  Serial.println(distanceL);
}

void ask_pin_R() // test right distance
{
  distanceR = get_distance(10);
  Serial.print("R distance:");
  Serial.println(distanceR);
}

void go(int valRB, int valRF, int valLB, int valLF, int delayTime) {
  digitalWrite(pinRB,valRB);
  digitalWrite(pinRF,valRF);
  digitalWrite(pinLB,valLB);
  digitalWrite(pinLF,valLF);
  delay(delayTime);
}

void advance(int dt)  {go(LOW,HIGH,LOW,HIGH,dt);} // go forward
void retreat(int dt)  {go(HIGH,LOW,HIGH,LOW,dt);} // go back
void turnL(int dt)    {go(LOW,HIGH,HIGH,LOW,dt);} // turn right
void turnR(int dt)    {go(HIGH,LOW,LOW,HIGH,dt);} // turn left
void stopp(int dt)    {go(LOW,LOW,LOW,LOW,dt);} // stop

void loop()
{
  myservo.write(90); // center servo
  detection();
  //valDirection=0;  // uncomment this to prevent wheel movement

  switch (valDirection) {
    case 2: // Bgo - Backward
      retreat(900);
      turnL(100);
      Serial.print(" Reverse ");
      break;
    case 4: // Lgo - Turn Left
      retreat(300);
      turnL(300);
      Serial.print(" Left ");
      break;
    case 6: // Rgo - Turn Right
      retreat(300);
      turnR(300);
      Serial.print(" Right ");
      break;
    case 8: // Fgo - Forward
      advance(15);
      Serial.print(" Advance ");
      break;
    default:
      //Serial.print(" Invalid ");
      break;
  }
}
