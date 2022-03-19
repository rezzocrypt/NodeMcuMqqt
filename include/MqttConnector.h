#ifndef MqttConnector_h
#define MqttConnector_h

#include <PubSubClient.h>
#include <WifiAutoConnector.h>
#include <EEPROM.h>
#include <mString.h>

// Читаем MQTT сообщения
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Message received: " + String(topic));
}

class MqttConnector {
    private:
        WiFiClient wClient = wifiConnector.wifiClient;
        bool checkMqttIp(IPAddress ip){
            return wClient.connect(ip, mqttPort);
        }

    public:
        WifiAutoConnector wifiConnector;

        //Хранение настроек IP адреса в EEPROM
        int MQTT_CONFIG_ADDRESS = 100;
        // порт mqtt про умолчанию
        int mqttPort = 1883;
        // адрес mqtt сервера
        IPAddress mqttIp;

        MqttConnector(){
            EEPROM.get(MQTT_CONFIG_ADDRESS, mqttIp);
        }

        // поиск mqtt сервера в сети
        // возвращает IP адрес
        IPAddress MqttServerIp() {
            unsigned currentTimeout = wClient.getTimeout();
            wClient.setTimeout(80);
            bool needFind = !mqttIp || !checkMqttIp(mqttIp);
            if(needFind){
                Serial.println("Find Mqtt");
                IPAddress currentIP = WiFi.localIP();
                for (int i = 1; i <= 255; i++) {
                    currentIP[3] = i;
                    Serial.print("chech Ip: ");Serial.println(currentIP);
                    if (checkMqttIp(currentIP)) {
                        mqttIp = currentIP;
                        EEPROM.put(MQTT_CONFIG_ADDRESS, currentIP);
                        break;
                    }
                }
            }
            wClient.setTimeout(currentTimeout);
            Serial.println("MQTT Ip: " + mqttIp.toString());
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
};
#endif