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
const int ledPin2 = 11;
const int ledPin1 = 12;
boolean isLedOn1 = false;
boolean isLedOn2 = false;
const int servoPin = 8;

const float CALIBRATION_FACTOR = 410.0; // Faktor kalibrasi (konversi dari raw value ke gram)

// Pengaturan penyaringan digital
const int NUM_SAMPLES = 10; // Jumlah sampel yang diambil untuk penyaringan rata-rata
float filteredWeight = 0.0;
// Pengaturan batas histeresis
const float HYSTERESIS_THRESHOLD = 10.0; // Batas histeresis dalam gram
float previousWeight = 0.0;

Servo myservo;
HX711 scale;
int servoOpenCount = 0;
int reset = 0;

void setup() {
  Serial.begin(9600);
  myservo.write(0);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  myservo.attach(servoPin);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);  // Ganti dengan faktor kalibrasi sesuai dengan load cell Anda

  // Mengatur tara awal (jika diperlukan)
  scale.tare();
}
void loop(){
  String minta = "";
  while (Serial.available()>0)
  {
    minta += char(Serial.read());
  }
  minta.trim();
  if(minta =="Ya")
  {
    //kirim datanya
    kirimdata();
  }
  minta ="";
  delay(1000);
}
void kirimdata() {
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
  
  
if (distance1 < 50) {
  if (distance3 > 5 && filteredWeight < 1000) {
    tone(buzzerPin, 1500);
    delay(200);
    noTone(buzzerPin);
    myservo.write(180);
    delay(5000);  
    myservo.write(0);
    servoOpenCount++;
  } else {
    tone(buzzerPin, 1500);
    delay(200);
    noTone(buzzerPin);
    tone(buzzerPin, 1500);
    delay(200);
    noTone(buzzerPin);
    myservo.write(0);
  }
}
if (distance3 <= 5){
  digitalWrite(ledPin1, HIGH);
  isLedOn1 = true;
  if(distance2 <= 5){
    myservo.write(180);
    delay(30000);
    myservo.write(0);
    reset++;
    servoOpenCount++;
    resetProgram();
  }
}
else{
  digitalWrite(ledPin1, LOW);
  isLedOn1 = false;
}
  if (scale.is_ready()) {
    float rawWeight = scale.get_units();

    // Penyaringan digital dengan moving average
    filteredWeight = filteredWeight * (NUM_SAMPLES - 1) / NUM_SAMPLES + rawWeight / NUM_SAMPLES;

    // Batas histeresis
    if (abs(filteredWeight - previousWeight) > HYSTERESIS_THRESHOLD) {
      // Pembacaan tidak valid, abaikan
      Serial.println("Pembacaan tidak valid");
    } else {
    // Memastikan filteredWeight tidak negatif
    if (filteredWeight < 0) {
      filteredWeight = 0;
    }
      if (filteredWeight >= 1000) {
        digitalWrite(ledPin2, HIGH);
        isLedOn2 = true;
        if(distance2 <= 5){
          myservo.write(180);
          delay(30000);
          myservo.write(0);
          reset++;
          servoOpenCount++;
          resetProgram();
        }
      } else {
        digitalWrite(ledPin2, LOW);
        isLedOn2 = false;
      }
    }
    previousWeight = filteredWeight;
  }

  String datakirim =  String(servoOpenCount) + "#" + String(distance1) + "#" + String(distance2) + "#" + String(distance3) + "#" + String(filteredWeight,1) + "#" + String(isLedOn1) + "#" + String(isLedOn2) + "#" + String(reset);
  Serial.println(datakirim);
  delay(1000);
}

int getServoOpenCount(){
  return servoOpenCount;
}
int getReset(){
  return reset;
}
void resetProgram() {
  isLedOn1 = false;
  isLedOn2 = false;
  delay(30000);
  filteredWeight = 0.0;
  previousWeight = 0.0;
  noTone(buzzerPin);
  myservo.write(0);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  // Membersihkan Serial Buffer
  while (Serial.available() > 0) {
    Serial.read();
  }
}