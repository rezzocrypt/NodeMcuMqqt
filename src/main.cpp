#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <GyverBME280.h>
#include <mString.h>

const int mqttPort = 1883;
const int timeout = 60 * 1000;
const int pin=2;

typedef struct {
  double temperature;
  double humidity;
  int pressure;
} BME280Data;

BME280Data ReadBMEData(){
  static GyverBME280 bme;
  BME280Data Result;
  if (bme.begin()) {
    Result.temperature = bme.readTemperature();
    Result.humidity = bme.readHumidity();
    Result.pressure = pressureToMmHg(bme.readPressure());
  }
  return Result;
}

char* GetDeviceName(){
  static char DeviceName[20];
  if(strlen(DeviceName) == 0){
    String mac = WiFi.macAddress();
    mac.replace(':', '_');
    mac.toCharArray(DeviceName, sizeof(DeviceName));
    Serial.print("set DeviceName: "); 
    Serial.println(DeviceName);
  }
  return DeviceName;
}

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
  Serial.print("time: ");
  Serial.println((millis()-timer_start) / 1000.0, 3);
  digitalWrite(pin, true);
  //delay(timeout);
  //ESP.deepSleep(timeout, WAKE_RF_DISABLED);
  ESP.deepSleep(timeout * 1000, RF_DEFAULT);
}