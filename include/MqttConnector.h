#ifndef MqttConnector_h
#define MqttConnector_h

#include <Arduino.h>
#include <PubSubClient.h>
#include <WifiAutoConnector.h>
#include <EEPROM.h>
#include <mString.h>

// размер эмулируемой EEPROM, должен покрывать все используемые адреса
static const int EEPROM_SIZE = 512;

// Читаем MQTT сообщения
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("Message received: " + String(topic));
}

class MqttConnector {
    public:
        // Автоматическое подключение к WIFI.
        // Объявлен первым в классе: порядок инициализации членов идёт по порядку
        // объявления, и все, что ниже обращается к wifiConnector, обязано идти после него.
        WifiAutoConnector wifiConnector;

    private:
        // Сокет используется и сканером сети, и MQTT клиентом.
        // Ссылка, чтобы не плодить второе подключение к брокеру.
        WiFiClient& wClient;

        bool checkMqttIp(IPAddress ip){
            bool found = wClient.connect(ip, mqttPort);
            // сокет сканера закрываем сразу, дальше его переиспользует MQTT клиент
            wClient.stop();
            return found;
        }

    public:
        // адрес в EEPROM для хранения IP mqtt сервера
        static const int MQTT_CONFIG_ADDRESS = 100;
        // порт mqtt про умолчанию
        int mqttPort = 1883;
        // адрес mqtt сервера
        IPAddress mqttIp;

        MqttConnector() : wClient(wifiConnector.wifiClient) {
            // EEPROM.begin обязателен до первого get/put на ESP8266
            EEPROM.begin(EEPROM_SIZE);
            EEPROM.get(MQTT_CONFIG_ADDRESS, mqttIp);
        }

        // поиск mqtt сервера в сети
        // возвращает IP адрес
        IPAddress MqttServerIp() {
            unsigned currentTimeout = wClient.getTimeout();
            // короткий таймаут, иначе скан 255 адресов займёт минуты
            wClient.setTimeout(80);
            bool needFind = !mqttIp || !checkMqttIp(mqttIp);
            if(needFind){
                Serial.println("Find Mqtt");
                // сохранённый адрес не подошёл, забываем его, чтобы не вернуть заведомо мёртвый
                mqttIp = IPAddress();
                IPAddress currentIP = WiFi.localIP();
                for (int i = 1; i <= 255; i++) {
                    currentIP[3] = i;
                    Serial.print("chech Ip: ");Serial.println(currentIP);
                    if (checkMqttIp(currentIP)) {
                        mqttIp = currentIP;
                        EEPROM.put(MQTT_CONFIG_ADDRESS, currentIP);
                        EEPROM.commit();
                        break;
                    }
                }
                if(!mqttIp)
                    Serial.println("Mqtt server not found");
            }
            wClient.setTimeout(currentTimeout);
            Serial.println("MQTT Ip: " + mqttIp.toString());
            return mqttIp;
        }

        // отправка данных с датчика на mqtt сервер
        void SendReport(const char* sensorName, const char* json){
            IPAddress mqttIP = MqttServerIp();
            if(!mqttIP)
                return;
            PubSubClient MqttClient(mqttIP, mqttPort, mqtt_callback, wClient);
            const char* DeviceName = wifiConnector.GetDeviceName();
            if (MqttClient.connect(DeviceName)){
                mString<65> topic;
                topic += DeviceName;
                topic +="/";
                topic += sensorName;
                MqttClient.publish(topic.buf, json);
                MqttClient.disconnect();
                Serial.println("Send complete");
            }
            else {
                Serial.println("Mqtt connect fail, state: " + String(MqttClient.state()));
            }
        }
};
#endif
