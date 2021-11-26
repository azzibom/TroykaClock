#include <TroykaRTC.h> // библиотека для работы с часами реального времени
#include <TroykaDHT.h> // библиотека для работы с датчиками серии DHT
#include <QuadDisplay2.h> // Подключаем библиотеку для работы с дисплеем
#include <TroykaButton.h> // Подключаем библиотеку для работы с кнопкой

#define DHT_PIN 4
#define QD_PIN 10
#define BTN_PIN 11

RTC clock; // создаём объект для работы с часами реального времени

// создаём объект класса DHT
// передаём номер пина к которому подключён датчик и тип датчика
// типы сенсоров: DHT11, DHT21, DHT22
DHT dht(DHT_PIN, DHT11);

QuadDisplay qd(QD_PIN); // создаём объект класса QuadDisplay и передаём номер пина CS

TroykaButton button(BTN_PIN);

void setup() {
  Serial.begin(9600);
  qd.begin(); // инициализация дисплея
  clock.begin(); // инициализация часов
  dht.begin(); // инициализация термометра
  dht.read();
  pinMode(BTN_PIN, INPUT);
  button.begin();

  // метод установки времени и даты в модуль вручную
  //  clock.set(10,25,45,27,07,2005,THURSDAY);
  // метод установки времени и даты автоматически при компиляции
  //  clock.set(__TIMESTAMP__);
}

uint32_t dhtTimer; // таймер для dht
int dhtPeriod = 2000; // период чтения данных с dht

uint32_t rtcTimer; // таймер для rtc
int rtcPeriod = 500; // период чтения данных с rtc

void loop() {
  // запрашиваем данные с часов
  clock.read();
  // считываем показания часов и минут в переменные
  int hour = clock.getHour();
  int minute = clock.getMinute();
  int second = clock.getSecond();

  uint32_t dhtTimeLeft = millis() - dhtTimer;
  if (dhtTimeLeft >= dhtPeriod) {
    dhtTimer += dhtPeriod * (dhtTimeLeft / dhtPeriod);
    dht.read();
  }

//  uint32_t rtcTimeLeft = millis() - rtcTimer;
//  if (rtcTimeLeft >= rtcPeriod) {
//    rtcTimer += rtcPeriod * (rtcTimeLeft / rtcPeriod);

    if ((second > 30 && second < 35) || (second > 0 && second < 5) ) {
      // проверяем состояние данных
      switch (dht.getState()) {
        case DHT_OK: { // всё OK
            qd.displayTemperatureC(dht.getTemperatureC()); // выводим только температуту т к влажность беспощадно врет
            break;
          }
        case DHT_ERROR_CHECKSUM: { // ошибка контрольной суммы
//            Serial.println("Checksum error");
            qd.displayDigits(QD_E, QD_r, QD_r, QD_1); // Err1
            break;
          }
        case DHT_ERROR_TIMEOUT: { // превышение времени ожидания
//            Serial.println("Time out error");
            qd.displayDigits(QD_E, QD_r, QD_r, QD_2); // Err2
            break;
          }
        case DHT_ERROR_NO_REPLY: { // данных нет, датчик не реагирует или отсутствует
//            Serial.println("Sensor not connected");
            qd.displayDigits(QD_E, QD_r, QD_r, QD_0); // Err0
            break;
          }
      }
    } else {
      qd.displayScore(hour, minute, true);
    }
//  }

  // считываем данные с кнопки
  button.read();

  // определяем короткое нажатие кнопки
  if (button.isClick()) {
    //    Serial.println("Key is clicked");
    clock.setMinute(minute - 1);
  }

}
