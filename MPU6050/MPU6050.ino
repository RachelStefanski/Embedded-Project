#include <Wire.h>
#include <MPU9250_asukiaaa.h>

// הגדרת פינים של I2C ל־ESP32 – ברוב הלוחות:
// SDA = GPIO 21, SCL = GPIO 22
#define SDA_PIN 21
#define SCL_PIN 22

MPU9250_asukiaaa mySensor;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // התחלת I2C עם הפינים המתאימים ל־ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  Serial.println("MPU9250 מתחיל...");
}

void loop() {
  mySensor.accelUpdate();

  Serial.print("תאוצה X: ");
  Serial.print(mySensor.accelX());
  Serial.print(" | Y: ");
  Serial.print(mySensor.accelY());
  Serial.print(" | Z: ");
  Serial.println(mySensor.accelZ());

  delay(500);
}
