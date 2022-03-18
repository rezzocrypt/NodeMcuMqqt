#ifndef BMEReader_h
#define BMEReader_h

#include <GyverBME280.h>

// структура с параметрами от датчика BME280
typedef struct {
  double temperature;
  double humidity;
  int pressure;
} BME280Data;

// функция чтения данных с датчика BME280
BME280Data ReadBMEData(){
  static GyverBME280 bme;
  BME280Data scanResult;
  if (bme.begin()) {
    scanResult.temperature = bme.readTemperature();
    scanResult.humidity = bme.readHumidity();
    scanResult.pressure = pressureToMmHg(bme.readPressure());
  }
  return scanResult;
}
#endif