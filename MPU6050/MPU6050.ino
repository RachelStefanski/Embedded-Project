// #include <Wire.h>
// #include <MPU9250_asukiaaa.h>

// // הגדרת פינים של I2C ל־ESP32 – ברוב הלוחות:
// // SDA = GPIO 21, SCL = GPIO 22
// #define SDA_PIN 21
// #define SCL_PIN 22

// MPU9250_asukiaaa mySensor;

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   // התחלת I2C עם הפינים המתאימים ל־ESP32
//   Wire.begin(SDA_PIN, SCL_PIN);
//   mySensor.setWire(&Wire);
//   mySensor.beginAccel();
//   mySensor.beginGyro();
//   mySensor.beginMag();

//   Serial.println("MPU9250 מתחיל...");
// }

// void loop() {
//   mySensor.accelUpdate();

//   Serial.print("תאוצה X: ");
//   Serial.print(mySensor.accelX());
//   Serial.print(" | Y: ");
//   Serial.print(mySensor.accelY());
//   Serial.print(" | Z: ");
//   Serial.println(mySensor.accelZ());

//   delay(500);
// }


#include <Wire.h>
#include <MPU9250_asukiaaa.h>

#define SDA_PIN 21
#define SCL_PIN 22

MPU9250_asukiaaa mySensor;

float baseX = 0, baseY = 0, baseZ = 0;
float accelThreshold = 2.5;  // תאוצה מופרזת (בערכים של G)
float flipThreshold = 0.7;   // זווית סטייה ממצב ישר (ברדיאנים)
float directionalAccelThreshold = 1.0;  // סף לזיהוי תאוצה חזקה בצד מסוים

void calibrateBaseline() {
  int samples = 100;
  baseX = baseY = baseZ = 0;

  for (int i = 0; i < samples; i++) {
    mySensor.accelUpdate();
    baseX += mySensor.accelX();
    baseY += mySensor.accelY();
    baseZ += mySensor.accelZ();
    delay(10);
  }

  baseX /= samples;
  baseY /= samples;
  baseZ /= samples;

  Serial.println("✔️ כיול מצב ישר הושלם:");
  Serial.print("X: "); Serial.print(baseX);
  Serial.print(" | Y: "); Serial.print(baseY);
  Serial.print(" | Z: "); Serial.println(baseZ);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);

  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  Serial.println("⌛ מתחיל כיול מצב ישר...");
  calibrateBaseline();
}

void loop() {
  mySensor.accelUpdate();

  float ax = mySensor.accelX();
  float ay = mySensor.accelY();
  float az = mySensor.accelZ();

  Serial.print("תאוצה X: "); Serial.print(ax);
  Serial.print(" | Y: "); Serial.print(ay);
  Serial.print(" | Z: "); Serial.println(az);

  // בדיקת תאוצה מופרזת כללית
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);
  if (totalAccel > accelThreshold) {
    Serial.println("⚠️ תאוצה מופרזת כללית!");
  }

  // זיהוי כיוון התאוצה והצעת כיוון הפוך
  if (ax > directionalAccelThreshold) {
    Serial.println("↩️ תאוצה חזקה ימינה - כוון שמאלה");
  } else if (ax < -directionalAccelThreshold) {
    Serial.println("↪️ תאוצה חזקה שמאלה - כוון ימינה");
  }

  if (ay > directionalAccelThreshold) {
    Serial.println("⬇️ תאוצה חזקה קדימה - כוון אחורה");
  } else if (ay < -directionalAccelThreshold) {
    Serial.println("⬆️ תאוצה חזקה אחורה - כוון קדימה");
  }

  // בדיקת התהפכות
  float dotProduct = ax * baseX + ay * baseY + az * baseZ;
  float magnitudeCurrent = sqrt(ax * ax + ay * ay + az * az);
  float magnitudeBase = sqrt(baseX * baseX + baseY * baseY + baseZ * baseZ);

  float cosAngle = dotProduct / (magnitudeCurrent * magnitudeBase);
  float angle = acos(cosAngle);  // ברדיאנים

  if (angle > flipThreshold) {
    Serial.println("⚠️ זוהתה התהפכות או סטייה חדה מהמצב היציב!");
  }

  delay(500);
}
