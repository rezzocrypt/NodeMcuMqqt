#ifndef MqttConnector_h
#define MqttConnector_h

#include <PubSubClient.h>
#include <WifiAutoConnector.h>
#include <mString.h>

// Читаем MQTT сообщения
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Message received: " + String(topic));
}

class MqttConnector {
    public:
        // порт mqtt про умолчанию
        int mqttPort = 1883;
        WifiAutoConnector wifiConnector;

        // поиск mqtt сервера в сети
        // возвращает IP адрес
        IPAddress MqttServerIp() {
            static IPAddress mqttIp;
            WiFiClient wClient = wifiConnector.wifiClient;
            unsigned currentTimeout = wClient.getTimeout();
            wClient.setTimeout(80);
            bool needFind = !mqttIp || !wClient.connect(mqttIp, mqttPort);
            if(needFind){
                Serial.println("Find Mqtt");
                IPAddress currentIP = WiFi.localIP();
                for (int i = 1; i <= 255; i++) {
                    currentIP[3] = i;
                    if (wClient.connect(currentIP, mqttPort)) {
                        mqttIp = currentIP;
                        Serial.println("MQTT Ip: " + mqttIp.toString());
                        break;
                    }
                }
            }
            wClient.setTimeout(currentTimeout);
            return mqttIp;
        }

        // отправка данных с датчика на mqtt сервер
        void SendReport(const char* sensorName, char* json){
            IPAddress mqttIP = MqttServerIp();
            if(!mqttIP)
                return;
            PubSubClient MqttClient(mqttIP, MqttConnector::mqttPort, mqtt_callback, wifiConnector.wifiClient);
            char* DeviceName = wifiConnector.GetDeviceName();
            if (MqttClient.connect(DeviceName)){
                mString<65> topic;
                topic += DeviceName;
                topic +="/";
                topic += sensorName;
                MqttClient.publish(topic.buf, json);
                MqttClient.disconnect();
                Serial.println("Send complete");
            }
        }
    private:
};
#endif