#include <TroykaRTC.h> // библиотека для работы с часами реального времени
#include <TroykaDHT.h> // библиотека для работы с датчиками серии DHT
#include <QuadDisplay2.h> // Подключаем библиотеку для работы с дисплеем

#define DHT_PIN 4
#define QD_PIN 10

RTC clock; // создаём объект для работы с часами реального времени

// создаём объект класса DHT
// передаём номер пина к которому подключён датчик и тип датчика
// типы сенсоров: DHT11, DHT21, DHT22
DHT dht(DHT_PIN, DHT11);

QuadDisplay qd(QD_PIN); // создаём объект класса QuadDisplay и передаём номер пина CS

void setup() {
  qd.begin(); // инициализация дисплея
  clock.begin(); // инициализация часов
  dht.begin(); // инициализация термометра/

  // метод установки времени и даты в модуль вручную
  //  clock.set(10,25,45,27,07,2005,THURSDAY);
  // метод установки времени и даты автоматически при компиляции
  //  clock.set(__TIMESTAMP__);
}

bool dhtRead = false; // флаг чтения данных с DHT

void loop() {
  // запрашиваем данные с часов
  clock.read();
  // считываем показания часов и минут в переменные
  int hour = clock.getHour();
  int minute = clock.getMinute();
  int second = clock.getSecond();

  if ((second > 30 && second <= 35) || (second > 0 && second <= 5) ) {
    // считывание данных с датчика
    if (!dhtRead) {
      dht.read();
      dhtRead = true;
    }
    // проверяем состояние данных
    switch (dht.getState()) {

      case DHT_OK: { // всё OK
          qd.displayClear();
          // выводим показания влажности и температуры
          if ((second > 30 && second <= 33) || (second > 0 && second <= 3)) {
            qd.displayTemperatureC(dht.getTemperatureC());
          } else {
            qd.displayHumidity(dht.getHumidity());
          }
          break;
        }
      case DHT_ERROR_CHECKSUM: { // ошибка контрольной суммы
          Serial.println("Checksum error");
          break;
        }
      case DHT_ERROR_TIMEOUT: { // превышение времени ожидания
          Serial.println("Time out error");
          break;
        }
      case DHT_ERROR_NO_REPLY: { // данных нет, датчик не реагирует или отсутствует
          Serial.println("Sensor not connected");
          break;
        }
    }
  } else {
    dhtRead = false;
    qd.displayScore(hour, minute, true);
  }
  delay(500);
}