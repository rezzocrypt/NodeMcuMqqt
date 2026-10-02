#ifndef BMEReader_h
#define BMEReader_h

#include <GyverBME280.h>

// структура с параметрами от датчика BME280
typedef struct {
  double temperature;
  double humidity;
  int pressure;
  // false, если датчик не инициализирован или не отдал данные
  bool valid;
} BME280Data;

// функция чтения данных с датчика BME280
BME280Data ReadBMEData(){
  static GyverBME280 bme;
  BME280Data scanResult = {0, 0, 0, false};
  static bool started = false;
  if (!started)
    started = bme.begin();
  if (started) {
    scanResult.temperature = bme.readTemperature();
    scanResult.humidity = bme.readHumidity();
    scanResult.pressure = pressureToMmHg(bme.readPressure());
    scanResult.valid = true;
  }
  return scanResult;
}
#endif