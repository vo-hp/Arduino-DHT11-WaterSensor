#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobot_DHT11.h>

DFRobot_DHT11 DHT;
#define DHT11_PIN 10

LiquidCrystal_I2C lcd(0x27, 16, 2);

int val;
int percent;
const int HUMIDITY_THRESHOLD = 80;
const int WATERLEVEL_THRESHOLD = 50;
const int TEMPERATURE_THRESHOLD = 40;

bool isLcdBlinking = false;
bool isLcdOn = false;

unsigned long currentReadDHTSensorMillis;
unsigned long previousReadDHTSensorMillis = 0;
const unsigned long intervalReadDHTSensor = 1000;

unsigned long currentReadWaterSensorMillis;
unsigned long previousReadWaterSensorMillis = 0;
const unsigned long intervalReadWaterSensor = 1000;

unsigned long currentBlinkMillis;
unsigned long previousBlinkMillis = 0;
const unsigned long intervalBlink = 200;

byte trong[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

void readDHTSensor() {
  currentReadDHTSensorMillis = millis();
  if (currentReadDHTSensorMillis - previousReadDHTSensorMillis >= intervalReadDHTSensor) {
    previousReadDHTSensorMillis = currentReadDHTSensorMillis;
    DHT.read(DHT11_PIN);
    Serial.print("temp: ");
    Serial.println(DHT.temperature);
    Serial.print("humi: ");
    Serial.println(DHT.temperature);
  }
}

void readWaterSensor() {
  currentReadWaterSensorMillis = millis();
  if (currentReadWaterSensorMillis - previousReadWaterSensorMillis >= intervalReadWaterSensor) {
    previousReadWaterSensorMillis = currentReadWaterSensorMillis;
    val = analogRead(A0);
    Serial.print("water level: ");
    Serial.println(val);
  }
}

void printLcd() {
  percent = map(val, 0, 700, 0, 100);
  lcd.setCursor(9, 0);
  lcd.print("temp:");
  lcd.setCursor(14, 0);
  lcd.print(DHT.temperature);
  lcd.setCursor(0, 1);
  lcd.print("level:");
  lcd.setCursor(6, 1);
  lcd.print(percent);
  if (percent < 10) {
    lcd.setCursor(7, 1);
    lcd.write(0);
  }
  if (percent < 100) {
    lcd.setCursor(8, 1);
    lcd.write(0);
  }
  lcd.setCursor(9, 1);
  lcd.print("humi:");
  lcd.setCursor(14, 1);
  lcd.print(DHT.humidity);
}

void printWarningLcd() {
  lcd.setCursor(0, 0);
  lcd.print("warning");
}

void clearWarningLcd() {
  lcd.setCursor(0, 0);
  lcd.print("       ");
}

void checkAndBlinkLcd() {
  if (DHT.humidity >= HUMIDITY_THRESHOLD || DHT.temperature >= TEMPERATURE_THRESHOLD || percent >= WATERLEVEL_THRESHOLD) {
    currentBlinkMillis = millis();
    printWarningLcd();
    if (currentBlinkMillis - previousBlinkMillis >= intervalBlink) {
      previousBlinkMillis = currentBlinkMillis;
      if (isLcdOn) {
        lcd.noBacklight();
        isLcdOn = false;
      } 
      else {
        lcd.backlight();
        isLcdOn = true;
      }
    } 
  }  
  else {
    clearWarningLcd();
    lcd.backlight();
  }
}

  void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, trong);
    pinMode(A0, INPUT);
  }

  void loop() {
    readDHTSensor();
    readWaterSensor();
    printLcd();
    checkAndBlinkLcd();
  }
