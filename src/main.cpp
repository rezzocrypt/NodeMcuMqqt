#include <BMEReader.h>
#include <MqttConnector.h>

// сон после отправки сообщения на mqtt
const int timeout = 60 * 1000;

// пин светодиода
const int pin=2;

// работа с mqqt
MqttConnector MqqtConnector;

// инициализация устройства
void setup() {
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  Serial.println("NodeMCU v3");
  MqqtConnector.wifiConnector.InitWifi();
}

void loop() {
  digitalWrite(pin, false);

    BME280Data data = ReadBMEData();
    // собираем json данные для mqqt
          mString<65> json;
          json += "{\"tempC\":";
          json += data.temperature;
          json += ",\"humidity\":";
          json += data.humidity;
          json += ",\"pressure\":";
          json += data.pressure;
          json += "}";
    MqqtConnector.SendReport(json.buf);

  digitalWrite(pin, true);
  delay(timeout);
  //ESP.deepSleep(timeout, WAKE_RF_DISABLED);
  //ESP.deepSleep(timeout * 1000, RF_DEFAULT);
}