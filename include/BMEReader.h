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
  BME280Data Result;
  if (bme.begin()) {
    Result.temperature = bme.readTemperature();
    Result.humidity = bme.readHumidity();
    Result.pressure = pressureToMmHg(bme.readPressure());
  }
  return Result;
}
#endif