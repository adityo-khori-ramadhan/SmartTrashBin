#include <Servo.h>
#include "HX711.h"

// Pin sensor ultrasonik
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
boolean isServoRotated = false;
// Pin servo motor
const int servoPin = 8;

// Inisialisasi konstanta kalibrasi
const float CALIBRATION_FACTOR = 460.0; // Faktor kalibrasi (konversi dari raw value ke gram)

// Pengaturan penyaringan digital
const int NUM_SAMPLES = 10; // Jumlah sampel yang diambil untuk penyaringan rata-rata
float filteredWeight = 0.0;

// Pengaturan batas histeresis
const float HYSTERESIS_THRESHOLD = 10.0; // Batas histeresis dalam gram
float previousWeight = 0.0;

// Objek servo
Servo myservo;
HX711 scale;

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

// Mengatur kalibrasi skala load cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);  // Ganti dengan faktor kalibrasi sesuai dengan load cell Anda

  // Mengatur tara awal (jika diperlukan)
  scale.tare();
  // Memulai komunikasi Serial dengan baud rate 9600
  Serial.begin(9600);
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
  // Deklarasi variabel jarak sensor ultrasonik
  long duration1, distance1;
  long duration2, distance2;
  long duration3, distance3;

  // Mengukur jarak dengan sensor ultrasonik 1
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;


    // Mengukur jarak dengan sensor ultrasonik 2
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance2 = duration2 * 0.034 / 2;

  // Mengukur jarak dengan sensor ultrasonik 3
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  duration3 = pulseIn(echoPin3, HIGH);
  //distance3 = duration3 * 0.034 / 2;
  distance3 = 25 - (duration3 * 0.034 / 2);
  // Pengecekan objek dengan sensor ultrasonik 1
  if (distance1 < 50 && distance3 < 20 && filteredWeight < 500) {
    tone(buzzerPin, 2000);
    delay(200);
  
    // Berhenti mengeluarkan suara
    noTone(buzzerPin);
    
    // Menggerakkan servo ke posisi 180 derajat
    myservo.write(180);
    isServoRotated = true;
    delay(5000); // Tunggu selama 5 detik
  }
  else {
    myservo.write(0);
    isServoRotated = false;
  }

  // Pengecekan objek dengan sensor ultrasonik 3
  if (distance3 >= 20) {
    // Sensor ultrasonik 1 tidak berfungsi
    
    // Mengaktifkan lampu LED1
    digitalWrite(ledPin2, HIGH);
    isLedOn2 = true;
    
    if (distance2 <= 5 && filteredWeight >= 500) {
      // Menggerakkan servo ke posisi 180 derajat selama 10 detik
      myservo.write(180);
      isServoRotated = true;
      delay(10000); // Tunggu selama 10 detik
      myservo.write(0);
      isServoRotated = false;
    }
  }
  else {
    // Mematikan lampu LED1
    digitalWrite(ledPin2, LOW);
    isLedOn2 = false;
    if (distance2 <= 5 && filteredWeight >= 500) {
    // Menggerakkan servo ke posisi 180 derajat selama 10 detik
    myservo.write(180);
    isServoRotated = true;
    delay(10000); // Tunggu selama 10 detik
    myservo.write(0);
    isServoRotated = false;
    }
  }

  if (scale.is_ready()) {
    float rawWeight = scale.get_units(); // Mendapatkan berat dalam gram

    // Penyaringan digital dengan moving average
    filteredWeight = filteredWeight * (NUM_SAMPLES - 1) / NUM_SAMPLES + rawWeight / NUM_SAMPLES;

    // Batas histeresis
    if (abs(filteredWeight - previousWeight) > HYSTERESIS_THRESHOLD) {
      // Pembacaan tidak valid, abaikan
      Serial.println("Pembacaan tidak valid");
    } else {

      // Pengecekan berat 500 gram
      if (filteredWeight >= 500) {
        // Mengaktifkan lampu LED2
        myservo.write(0);
        digitalWrite(ledPin1, HIGH);
        isLedOn1 = true;
      } else {
        // Mematikan lampu LED2
        digitalWrite(ledPin1, LOW);
        isLedOn1 = false;
      }
    }

    previousWeight = filteredWeight;
    String datakirim = String(distance3) + "#" + String(filteredWeight, 1)+ "#" + String(isLedOn1) + "#" + String(isLedOn2) + "#" + String(isServoRotated);
    Serial.println(datakirim);
  }
  delay(200);
}
