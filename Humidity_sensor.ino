#include <TFT_eSPI.h> // ספרייה לתצוגת TFT על ESP32

TFT_eSPI tft = TFT_eSPI(); // יצירת אובייקט לתצוגה

const int moistureSensorDigitalPin = 2; // פין דיגיטלי של חיישן הרטיבות
const int moistureSensorAnalogPin = 36; // פין אנלוגי של חיישן הרטיבות (אם קיים)

bool isWet = false; // משתנה דגל כדי לעקוב אחר מצב הרטיבות הקודם

void setup() {
  Serial.begin(115200);
  Serial.println("Serial communication started!"); // הודעת אתחול תקשורת טורית
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  pinMode(moistureSensorDigitalPin, INPUT);
}

void loop() {
  // קריאה מהפין הדיגיטלי
  int digitalValue = digitalRead(moistureSensorDigitalPin);
  tft.setCursor(0, 20);
  tft.print("Digital Value: ");
  tft.println(digitalValue);
  Serial.print("Digital Value: ");
  Serial.println(digitalValue);

  // בדיקה אם החיישן מזהה רטיבות (ערך LOW)
  if (digitalValue == LOW && !isWet) {
    isWet = true;
    Serial.println("!!! DETECTED MOISTURE !!!");
    tft.setCursor(0, 80);
    tft.setTextColor(TFT_RED);
    tft.println("!!! MOISTURE !!!");
    tft.setTextColor(TFT_WHITE); // החזרת צבע הטקסט לבן
  } else if (digitalValue == HIGH && isWet) {
    isWet = false;
    tft.fillRect(0, 80, tft.width(), 20, TFT_BLACK); // מחיקת הודעת הרטיבות
  }

  // קריאה מהפין האנלוגי (אם קיים)
  #ifdef moistureSensorAnalogPin
    int analogValue = analogRead(moistureSensorAnalogPin);
    tft.setCursor(0, 60);
    tft.print("Analog Value:  ");
    tft.println(analogValue);
    Serial.print("Analog Value:  ");
    Serial.println(analogValue);
  #endif

  delay(1000); // עיכוב של שנייה
}