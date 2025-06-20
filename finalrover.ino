#include <Servo.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Ultrasonic sensors
#define trigPinBack 2
#define echoPinBack 3
#define trigPinLeft 4
#define echoPinLeft 5
#define trigPinRight 6
#define echoPinRight 7
#define trigPinFrontLeft 8
#define echoPinFrontLeft 9
#define trigPinFrontRight 10
#define echoPinFrontRight 11

// H-Bridge 
#define motorAIn1 12
#define motorAIn2 13
#define motorBIn3 A0
#define motorBIn4 A1
#define enA 9
#define enB 10

Servo clawServo;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_MPU6050 mpu;

long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration/2)/74;
  return distance;
}

void setup() {
  Serial.begin(9600);

  // ultrasonic sensor pins
  pinMode(trigPinBack, OUTPUT);
  pinMode(echoPinBack, INPUT);
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);
  pinMode(trigPinFrontLeft, OUTPUT);
  pinMode(echoPinFrontLeft, INPUT);
  pinMode(trigPinFrontRight, OUTPUT);
  pinMode(echoPinFrontRight, INPUT);

  // motor control pins
  pinMode(motorAIn1, OUTPUT);
  pinMode(motorAIn2, OUTPUT);
  pinMode(motorBIn3, OUTPUT);
  pinMode(motorBIn4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  // Set initial motor speeds
  analogWrite(enA, 200);
  analogWrite(enB, 200);

  // servo motor
  clawServo.attach(servoPin);
  clawServo.write(90); 

  //ToF sensor
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // gyroscope
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }
}

void loop() {
  long distanceBack = measureDistance(trigPinBack, echoPinBack);
  long distanceLeft = measureDistance(trigPinLeft, echoPinLeft);
  long distanceRight = measureDistance(trigPinRight, echoPinRight);
  long distanceFrontLeft = measureDistance(trigPinFrontLeft, echoPinFrontLeft);
  long distanceFrontRight = measureDistance(trigPinFrontRight, echoPinFrontRight);

  if (distanceFrontLeft < 20 || distanceFrontRight < 20) {
    stopMotors();
    if (distanceLeft > distanceRight) {
      turnLeft();
    } else {
      turnRight();
    }
  } else if (distanceLeft < 20) {

    turnRight();
  } else if (distanceRight < 20) {
  
    turnLeft();
  } else {
    
    moveForward();
  }

  delay(100);
}

void moveForward() {
  digitalWrite(motorAIn1, HIGH);
  digitalWrite(motorAIn2, LOW);
  digitalWrite(motorBIn3, HIGH);
  digitalWrite(motorBIn4, LOW);
}

void turnLeft() {
  digitalWrite(motorAIn1, LOW);
  digitalWrite(motorAIn2, HIGH);
  digitalWrite(motorBIn3, HIGH);
  digitalWrite(motorBIn4, LOW);
  delay(300); 
  stopMotors();
}

void turnRight() {
  digitalWrite(motorAIn1, HIGH);
  digitalWrite(motorAIn2, LOW);
  digitalWrite(motorBIn3, LOW);
  digitalWrite(motorBIn4, HIGH);
  delay(300); 
  stopMotors();
}

void stopMotors() {
  digitalWrite(motorAIn1, LOW);
  digitalWrite(motorAIn2, LOW);
  digitalWrite(motorBIn3, LOW);
  digitalWrite(motorBIn4, LOW);
}


void grabObject() {
  stopMotors();
  clawServo.write(0); 
  delay(1000); 
}

void deliverObject() {
  turnLeft();
  delay(1000); 
  stopMotors();
  moveForward(); 
  delay(2000); 
  stopMotors();
  clawServo.write(90); 
  delay(1000); 
}

