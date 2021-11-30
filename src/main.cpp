#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <mString.h>
#include <BMEReader.h>

// порт mqtt про умолчанию
const int mqttPort = 1883;
// сон после отправки сообщения на mqtt
const int timeout = 60 * 1000;
// пин светодиода
const int pin=2;

// генерация имени устройства на основе mac WIFI адаптера
char* GetDeviceName(){
  static char DeviceName[20];
  if(strlen(DeviceName) == 0){
    String mac = WiFi.macAddress();
    mac.replace(':', '_');
    mac.toCharArray(DeviceName, sizeof(DeviceName));
    Serial.print("DeviceName: "); 
    Serial.println(DeviceName);
  }
  return DeviceName;
}

// инициализация WIFI адаптера.
// пробует подключиться к заданной WIFI сети, если не получается инициализирует сервер.
void InitWifi(){
  WiFi.mode(WIFI_STA);
  std::vector<const char *> menu = {"wifi", "info"};
  WiFiManager wm;
  wm.setMenu(menu);
  wm.setClass("invert");
  if (!wm.autoConnect("MQTTDevice", "password")) {
    ESP.restart();
  }
  wm.setHostname(GetDeviceName());
}

// поиск mqtt сервера в сети
// возвращает IP адрес
IPAddress MqttServerIp() {
  static IPAddress mqttIp;
  WiFiClient wifiClient;
  unsigned currentTimeout = wifiClient.getTimeout();
  wifiClient.setTimeout(80);
  bool needFind = !mqttIp || !wifiClient.connect(mqttIp, mqttPort);
  if(needFind){
    Serial.println("Find Mqtt");
    IPAddress currentIP = WiFi.localIP();
    for (int i = 1; i <= 255; i++) {
      currentIP[3] = i;
      if (wifiClient.connect(currentIP, mqttPort)) {
        mqttIp = currentIP;
        Serial.println("MQTT Ip: " + mqttIp.toString());
        break;
      }
    }
  }
  wifiClient.setTimeout(currentTimeout);
  return mqttIp;
}

// Читаем MQTT сообщения
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message received: " + String(topic));
}

// отправка данных с датчика на mqtt сервер
void SendReport(BME280Data d){
  IPAddress mqttIP = MqttServerIp();
  if(!mqttIP)
    return;
  WiFiClient wifiClient;
  PubSubClient MqttClient(mqttIP, mqttPort, mqtt_callback, wifiClient);
  char* DeviceName = GetDeviceName();
  if (MqttClient.connect(DeviceName)){
    mString<50> topic;
    topic += "BME280/";
    topic += DeviceName;
    mString<65> json;
    json += "{\"tempC\":";
    json += d.temperature;
    json += ",\"humidity\":";
    json += d.humidity;
    json += ",\"pressure\":";
    json += d.pressure;
    json += "}";
    MqttClient.publish(topic.buf, json.buf);
    MqttClient.disconnect();
    Serial.println("Send complete");
  }
}

// инициализация устройства
void setup() {
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  Serial.println("NodeMCU v3");
  InitWifi();
}

void loop() {
  digitalWrite(pin, false);
  long timer_start = millis();
  //WiFi.setSleep(false);
  BME280Data data = ReadBMEData();
  SendReport(data);
  //WiFi.setSleep(true);
  Serial.print("time read and send data: ");
  Serial.println((millis()-timer_start) / 1000.0, 3);
  digitalWrite(pin, true);
  delay(timeout);
  //ESP.deepSleep(timeout, WAKE_RF_DISABLED);
  //ESP.deepSleep(timeout * 1000, RF_DEFAULT);
}