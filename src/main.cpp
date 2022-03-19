#include <BMEReader.h>
#include <MqttConnector.h>

// сон после отправки сообщения на mqtt
const int timeout = 120e6;

// пин светодиода
const int ledPin=2;

// работа с mqqt
MqttConnector MqqtConnector;

// инициализация устройства
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("NodeMCU v3");
  MqqtConnector.wifiConnector.InitWifi();
}

//Запуск фунций репортов с перехватом ошибок
void InvokeAction(void (*ptrF)(void)){
  try{ ptrF(); }
  catch(...) { }
}

//отправка данных с датчика BME280
void BMEReport(){
  BME280Data data = ReadBMEData();
  //не получены данные со счетчика
  if(data.pressure == 0 || data.temperature == 0 || data.humidity ==0)
    return;
  // собираем json данные для mqqt
    mString<65> json;
    json += "{\"tempC\":";
    json += data.temperature;
    json += ",\"humidity\":";
    json += data.humidity;
    json += ",\"pressure\":";
    json += data.pressure;
    json += "}";
  MqqtConnector.SendReport("BME280", json.buf);
}

void loop() {
  digitalWrite(ledPin, true);
  
  InvokeAction(BMEReport);

  digitalWrite(ledPin, false);
  ESP.deepSleep(timeout, RF_DEFAULT);
}