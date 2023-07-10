#include <SoftwareSerial.h>
#include  <ThingerESP8266.h>
#include <ESP8266WIFI.h>

SoftwareSerial DataSerial(12,13);

unsigned long previousMillis = 0;
const long interval = 3000;

//variabel array untuk data parsing
String arrData[5];

//konfigurasi thinger.io
#define USERNAME "adityokhori"
#define DEVICE_ID "Serial_ThingerIO"
#define DEVICE_CREDENTIAL "k@H3I5!gTwm67_0e"
//VARIABEL untuk thingerio
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

//KONFIGURASI WIFI
const char* ssid = "3x-5y+7=15";
const char* password = "hitungdulu";
int distance3;
float filteredWeight;
boolean isLedOn1, isLedOn2, isServoRotated;
//sediakan variabel menampung nilai sensor2, bera,t led1,led2 ke thingerio

void setup() {
  Serial.begin(9600);
  DataSerial.begin(9600);
  //start Wifi
  WiFi.begin(ssid, password);
  //hubungkan nodeMCU ke thinger
  thing.add_wifi(ssid, password);
  //data yang akan dikirim
  thing["Dataku"] >> [](pson & out)
  {
    out["distance3"] = distance3;
    out["Berat"] = filteredWeight;
    out["Lampu1"] = isLedOn1;
    out["Lampu2"] = isLedOn2;
    out["Servo"] = isServoRotated;
  };

}

void loop() {
  //picu pengiriman data ke thinger
thing.handle();
unsigned long currentMillis = millis();
if(currentMillis - previousMillis >= interval)
{
  //update previousMillis
  previousMillis = currentMillis;

  //prioritaskan pembacaan data dari arduino uno (hasil kiriman data)
  //baca data serial
  String data = "";
  while(DataSerial.available()>0)
  {
    data += char(DataSerial.read());
  }
  //buang spasi datanya
  data.trim();

  //uji data
  if(data != "")
  {
    //format data "10#20#30#50" = array (setelah parsing)
    //parsing data (pecah data)
    int index = 0;
    for (int i=0; i<= data.length(); i++)
    {
      char delimiter = '#';
      if(data[i] != delimiter)
          arrData[index] += data[i];
      else
          index++; //variabel index bertambah 1
    }

    //pastikan bahwa data yang dikirim lengkap
    //urutannya 0,1,2,3
    if(index == 4)
    {
      //tampilkan nilai sensor ke serial monitor
      Serial.println(arrData[0]);
      Serial.println(arrData[1]);
      Serial.println(arrData[2]);
      Serial.println(arrData[3]);
      Serial.println(arrData[4]);
      Serial.println();
    }
distance3 = arrData[0].toInt();
filteredWeight = arrData[1].toFloat();
isLedOn1 = arrData[2].toInt();
isLedOn2 = arrData[3].toInt();
isServoRotated = arrData[4].toInt();

    arrData[0] = "";
    arrData[1] = "";
    arrData[2] = "";
    arrData[3] = "";
    arrData[4] = "";
  }
  //minta data ke arduino
  DataSerial.println("Ya");
}

}
