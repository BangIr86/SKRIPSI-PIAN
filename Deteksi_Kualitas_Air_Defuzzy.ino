#include <OneWire.h>
#include <DallasTemperature.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLBp8DHjTM"
#define BLYNK_DEVICE_NAME "esp8266"
#define BLYNK_AUTH_TOKEN "CBncsXqIwKMVBE3gfuNJI44KN7sBhII6"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "nodeMcu";  //SSID HOSPOT
char pass[] = "Blynk123";  //PASS HOSPOT

BLYNK_WRITE(V0); //Suhu
BLYNK_WRITE(V1); //LCD
BLYNK_CONNECTED(){
  Blynk.syncVirtual(V0); //suhu
  Blynk.syncVirtual(V1); //LCD
}
WidgetLCD lcd(V1);

const int pinled = 5; //Led in NodeMCU at pin D1 nodeMcu
const int sensor_suhu = 2; //port D2 sensor DS18B20
OneWire oneWire (sensor_suhu);
DallasTemperature sensorsuhu(&oneWire);

float tegangan;
float suhu;

float rule0, rule1, rule2;
float out0, out1, out2;
float on = 10;
float off = 3;
float rule[3];
float out[3];

//keanggotaan suhu
float kgdingin [] = {15, 19, 24};
float kgnormal [] = {21, 27, 33};
float kgpanas [] = {30, 35, 40};

void setup() {
  pinMode(sensor_suhu, INPUT); //sensor suhu
  pinMode(pinled, OUTPUT); //OUTPUT LED
  Serial.begin(9600);
  sensorsuhu.begin();
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);
}

void loop() {
  Blynk.run(); //syntax wajib untuk jalankan blynk setup
  lcd.print(0, 0, "STATUS");
  
  sensorsuhu.requestTemperatures();
  suhu = sensorsuhu.getTempCByIndex(00);

  float defuzzy=Defuzzyfikasi();

  Serial.print("Defuzzyfikasi = ");
  Serial.println(defuzzy);
  lcd.print(8, 0 , defuzzy);
  Serial.print("SUHU AIR = ");
  Serial.println(suhu);
  Blynk.virtualWrite(V0, suhu);


  if (defuzzy >= 3.01){
    digitalWrite (pinled, HIGH);
    lcd.print(0, 1, "SUHU TDK NORMAL");
  }
  else if (defuzzy <= 3){
    digitalWrite (pinled, LOW);
    lcd.print(0, 1, "SUHU NORMAL....");
    delay (2000);
  } 
}

//FUZZY SUHU
float fzdingin (){
  if (suhu <kgdingin [0]){
    return 0;
  } else if (suhu >=kgdingin [0] && suhu <=kgdingin [1]){
    return (suhu - kgdingin [0])/(kgdingin [1] - kgdingin [0]);
  } else if (suhu >=kgdingin [1] && suhu  <= kgdingin [2]){
    return (kgdingin [2] - suhu)/(kgdingin [2] - kgdingin [1]);
  } else if (suhu > kgdingin [2]){
    return 0;
  }}
float fznormal (){
  if (suhu <kgnormal [0]){
    return 0;
  } else if (suhu >=kgnormal [0] && suhu <=kgnormal [1]){
    return (suhu - kgnormal [0])/(kgnormal [1] - kgnormal [0]);
  } else if (suhu >=kgnormal [1] && suhu  <= kgnormal [2]){
    return (kgnormal [2] - suhu)/(kgnormal [2] - kgnormal [1]);
  } else if (suhu > kgnormal [2]){
    return 0;
  }}
float fzpanas (){
  if (suhu <kgpanas [0]){
    return 0;
  } else if (suhu >=kgpanas [0] && suhu <=kgpanas [1]){
    return (suhu - kgpanas [0])/(kgpanas [1] - kgpanas[0]);
  } else if (suhu >=kgpanas [1] && suhu  <= kgpanas [2]){
    return (kgpanas [2] - suhu)/(kgpanas [2] - kgpanas [1]);
  } else if (suhu > kgpanas [2]){
    return 0;
  }}

//RULE
void rules (){
  rule[0]= (fzdingin());
  out[0]=on;
  rule[1]= (fznormal());
  out[1]=off;
  rule[2]= (fzpanas());
  out[2]=on;
}

//RUMUS DEFUZZY
float Defuzzyfikasi(){
  rules();
  float A=0; 
  float B=0;

  for (int i=0; i <=2; i++){
    A += out[i] * rule[i];
    B += rule[i];
  }
  return A/B;
} 
