#include <BMEReader.h>
#include <MqttConnector.h>
#include <TaskRunner.h>

// пин светодиода
const int ledPin=2;

// работа с mqqt
MqttConnector MqqtConnector;

// запускалка заданий на обработку
TaskRunner Runner;

// отправка данных с датчика BME280
bool BMEReport(){
  Serial.print("BMEReport =>");
  BME280Data data = ReadBMEData();
  //не получены данные со счетчика
  if(data.pressure == 0 || data.temperature == 0 || data.humidity == 0){
    Serial.println("read fail");
    return false;
  }
  // собираем json данные для mqqt
    mString<65> json;
    json += "{\"tempC\":";
    json += data.temperature;
    json += ",\"humidity\":";
    json += data.humidity;
    json += ",\"pressure\":";
    json += data.pressure;
    json += "}";
  Serial.println(json.buf);
  MqqtConnector.SendReport("BME280", json.buf);
  return true;
}

// инициализация устройства
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("NodeMCU v3");
  MqqtConnector.wifiConnector.InitWifi();

  Runner.AddTask("BMEReport", BMEReport, 120e6);
}

// цикл с засыпанием
void loop() {
  Serial.println("-------------");
  digitalWrite(ledPin, false);
  Runner.Invoke();
  digitalWrite(ledPin, true);
  Serial.println("Sleep");
  // учитываем, что deepSleep перезапустит плату и сработает только 1 задание
  ESP.deepSleep(Runner.Interval, RF_DEFAULT);
}