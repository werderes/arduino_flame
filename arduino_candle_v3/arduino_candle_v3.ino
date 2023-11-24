    #include <Wire.h>
    #include <ADXL345.h>
     
    ADXL345 accelerometer;
   
    void showRange(void)
    {
      Serial.print("Wybrany zakres pomiarowy: ");
     
      switch(accelerometer.getRange())
      {
        case ADXL345_RANGE_16G: Serial.println("+/- 16 g"); break;
        case ADXL345_RANGE_8G:  Serial.println("+/- 8 g");  break;
        case ADXL345_RANGE_4G:  Serial.println("+/- 4 g");  break;
        case ADXL345_RANGE_2G:  Serial.println("+/- 2 g");  break;
        default: Serial.println("Bledny zakres"); break;
      }
    }
     
    void showDataRate(void)
    {
      Serial.print("Wybrana szybkosc transmisji: ");
     
      switch(accelerometer.getDataRate())
      {
        case ADXL345_DATARATE_3200HZ: Serial.println("3200 Hz"); break;
        case ADXL345_DATARATE_1600HZ: Serial.println("1600 Hz"); break;
        case ADXL345_DATARATE_800HZ:  Serial.println("800 Hz");  break;
        case ADXL345_DATARATE_400HZ:  Serial.println("400 Hz");  break;
        case ADXL345_DATARATE_200HZ:  Serial.println("200 Hz");  break;
        case ADXL345_DATARATE_100HZ:  Serial.println("100 Hz");  break;
        case ADXL345_DATARATE_50HZ:   Serial.println("50 Hz");   break;
        case ADXL345_DATARATE_25HZ:   Serial.println("25 Hz");   break;
        case ADXL345_DATARATE_12_5HZ: Serial.println("12.5 Hz"); break;
        case ADXL345_DATARATE_6_25HZ: Serial.println("6.25 Hz"); break;
        case ADXL345_DATARATE_3_13HZ: Serial.println("3.13 Hz"); break;
        case ADXL345_DATARATE_1_56HZ: Serial.println("1.56 Hz"); break;
        case ADXL345_DATARATE_0_78HZ: Serial.println("0.78 Hz"); break;
        case ADXL345_DATARATE_0_39HZ: Serial.println("0.39 Hz"); break;
        case ADXL345_DATARATE_0_20HZ: Serial.println("0.20 Hz"); break;
        case ADXL345_DATARATE_0_10HZ: Serial.println("0.10 Hz"); break;
        default: Serial.println("Bleda szybkosc transmisji"); break;
      }
    }
const int ledPin = 5;  // Pin, do którego podłączona jest dioda LED
const int minMinValue = 30;  // Minimalna dolna jasność diody LED
const int maxMinValue = 200;  // Minimalna górna jasność diody LED
const int minMaxValue = 180;  // Maksymalna dolna jasność diody LED
const int maxMaxValue = 300;  // Maksymalna górna jasność diody LED
const int minDelay = 50;  // Minimalny czas migotania (w ms)
const int maxDelay = 500;  // Maksymalny czas migotania (w ms)

unsigned long previousMillis = 0;
int randomDelay = 0;
int currentBrightness = 0;
int targetBrightness = 0;
Vector previousAccel;  // Przechowuje poprzedni odczyt akcelerometru
void setup() {
      Serial.begin(9600);
        // Początkowa inicjalizacja poprzednich odczytów
      previousAccel = accelerometer.readRaw();
      // Inicjalizacja ADXL345
      Serial.println("Inicjalizacja ADXL345");
      if (!accelerometer.begin())
      {
        Serial.println("Nie odnaleziono ADXL345, sprawdz podlaczenie!");
        delay(500);
      }
     
      // Wybor zakresu pomiarowego
      // +/-  2G: ADXL345_RANGE_2G
      // +/-  4G: ADXL345_RANGE_4G
      // +/-  8G: ADXL345_RANGE_8G
      // +/- 16G: ADXL345_RANGE_16G
      accelerometer.setRange(ADXL345_RANGE_16G);
     
      // Wyswietlenie aktualnych parametrow
      showRange();
      showDataRate();
  pinMode(ledPin, OUTPUT);
  randomSeed(analogRead(0));  // Inicjalizacja generatora liczb losowych
  randomDelay = random(minDelay, maxDelay);  // Ustaw początkowy losowy czas migotania
  analogWrite(ledPin, 255-currentBrightness);  // Ustaw początkową jasność diody LED
}

void loop() {
        Vector raw = accelerometer.readRaw();

  // Oblicz różnicę między obecnymi a poprzednimi odczytami
  int deltaAccel = abs(raw.XAxis - previousAccel.XAxis) +
                   abs(raw.YAxis - previousAccel.YAxis) +
                   abs(raw.ZAxis - previousAccel.ZAxis);
    // Serial.println(deltaAccel);
      // Wyswietlenie danych surowych
     /* Serial.print(" Xraw = ");
      Serial.print(raw.XAxis);
      Serial.print(" Yraw = ");
      Serial.print(raw.YAxis);
      Serial.print(" Zraw: ");
      Serial.print(raw.ZAxis);
      Serial.println();*/
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= randomDelay) {
    previousMillis = currentMillis;
    float easeDuration = float(randomDelay);
    targetBrightness = random(random(minMinValue, max(minMinValue,maxMinValue-3*deltaAccel)), random(max(minMaxValue-3*deltaAccel,minMinValue), min(maxMaxValue-3*deltaAccel,minMaxValue)));  // Ustal nowy cel jasności
    randomDelay = random(minDelay, max(minDelay,maxDelay-3*deltaAccel));  // Ustaw nowy losowy czas migotania

    int deltaBrightness = targetBrightness - currentBrightness;
    int step = deltaBrightness > 0 ? 1 : -1;
    unsigned long startTime = currentMillis;


    while (currentMillis - startTime < randomDelay) {
      float t = float(currentMillis - startTime) / easeDuration;
      float easedT = 0.5 - 0.5 * cos(t * PI);  // Interpolacja "easyOut"
      int newBrightness = currentBrightness + int(easedT * deltaBrightness);
      analogWrite(ledPin, newBrightness);
      Serial.println(newBrightness);
      currentMillis = millis();
    }

    currentBrightness = targetBrightness;
  }
      // Zaktualizuj poprzednie odczyty
    previousAccel = raw;
}
