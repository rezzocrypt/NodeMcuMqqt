#ifndef WifiAutoConnector_h
#define WifiAutoConnector_h

#include <WiFiManager.h>
#include <ESP8266WiFi.h>

class WifiAutoConnector {
    public:
        //Клиент Wifi
        WiFiClient wifiClient;

        // генерация имени устройства на основе mac WIFI адаптера
        static char* GetDeviceName(){
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
    private:
};
#endif