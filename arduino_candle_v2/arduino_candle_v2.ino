#include <Wire.h>
#include <ADXL345.h>

ADXL345 accelerometer;

const int ledPin = 5;  // Pin, do którego podłączona jest dioda LED
const int minMinValue = 55;  // Minimalna dolna jasność diody LED
const int maxMinValue = 120;  // Minimalna górna jasność diody LED
const int minMaxValue = 120;  // Maksymalna dolna jasność diody LED
const int maxMaxValue = 255;  // Maksymalna górna jasność diody LED
const int minDelay = 50;  // Minimalny czas migotania (w ms)
const int maxDelay = 500;  // Maksymalny czas migotania (w ms)

unsigned long previousMillis = 0;
int randomDelay = 0;
int currentBrightness = 0;
int targetBrightness = 0;

void setup() {
  Serial.begin(9600);
  
  // Inicjalizacja ADXL345
  Serial.println("Inicjalizacja ADXL345");
  if (!accelerometer.begin()) {
    Serial.println("Nie odnaleziono ADXL345, sprawdź podłączenie!");
    delay(500);
  }
 
  // Wybór zakresu pomiarowego
  accelerometer.setRange(ADXL345_RANGE_16G);

  pinMode(ledPin, OUTPUT);
  randomSeed(analogRead(0));
  randomDelay = random(minDelay, maxDelay);
  analogWrite(ledPin, currentBrightness);
}

void loop() {
  Vector raw = accelerometer.readRaw();

  // Wyswietlenie danych surowych
  Serial.print(" Xraw = ");
  Serial.print(raw.XAxis);
  Serial.print(" Yraw = ");
  Serial.print(raw.YAxis);
  Serial.print(" Zraw: ");
  Serial.print(raw.ZAxis);
  Serial.println();

  // Oblicz wartość trzęsienia na podstawie odczytów z akcelerometru
  int shakeValue = abs(raw.XAxis) + abs(raw.YAxis) + abs(raw.ZAxis);

  // Ustaw parametry migotania na podstawie wartości trzęsienia
  int intensity = map(shakeValue, 0, 900, minMaxValue, maxMaxValue); // Dostosuj intensywność migotania
  int frequency = map(shakeValue, 0, 900, minDelay, maxDelay); // Dostosuj częstotliwość migotania

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= frequency) {
    previousMillis = currentMillis;
    float easeDuration = float(frequency);
    targetBrightness = random(minMinValue, intensity);
    randomDelay = frequency;

    int deltaBrightness = targetBrightness - currentBrightness;
    int step = deltaBrightness > 0 ? 1 : -1;
    unsigned long startTime = currentMillis;

    while (currentMillis - startTime < frequency) {
      float t = float(currentMillis - startTime) / easeDuration;
      float easedT = 0.5 - 0.5 * cos(t * PI);
      int newBrightness = currentBrightness + int(easedT * deltaBrightness);
      analogWrite(ledPin, newBrightness);
      currentMillis = millis();
    }

    currentBrightness = targetBrightness;
  }
}
