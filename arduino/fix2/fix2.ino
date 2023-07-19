#include <Servo.h>
#include "HX711.h"

const int trigPin1 = 2;
const int echoPin1 = 3;
const int trigPin2 = 5;
const int echoPin2 = 4;
const int trigPin3 = 7;
const int echoPin3 = 6;
const int LOADCELL_DOUT_PIN = 10;
const int LOADCELL_SCK_PIN = 9;
const int buzzerPin = 13;
const int ledPin1 = 11;
const int ledPin2 = 12;
boolean isLedOn1 = false;
boolean isLedOn2 = false;
const int servoPin = 8;


Servo myservo;

void setup() {
  // Mengatur pin sebagai input/output
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);

  // Mengatur pin servo sebagai output
  myservo.attach(servoPin);

  // Mengatur servo awal ke posisi 0 derajat
  myservo.write(0);

  // Memulai komunikasi Serial dengan baud rate 9600
  Serial.begin(9600);
}
void loop() {
  long duration1, distance1;
  long duration2, distance2;
  long duration3, distance3;

  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;

if (distance1 < 50 && distance3 > 5) {
    tone(buzzerPin, 2000);
    delay(200);
    noTone(buzzerPin);
    myservo.write(180);
    delay(5000);  
    myservo.write(0);
}

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;

  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  duration3 = pulseIn(echoPin3, HIGH);
  distance3 = duration3 * 0.034 / 2;

  if (distance3 <= 5) {

    if (distance2 <= 5) {
      tone(buzzerPin, 2000);
      delay(200);
      noTone(buzzerPin);
      myservo.write(180);
      delay(30000);
      myservo.write(0);
    }
  }
  else {

  }

  String datakirim =String(distance1) + "#" + String(distance2) + "#" + String(distance3);
  Serial.println(datakirim);
  delay(1000);
}
