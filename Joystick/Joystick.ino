#define JOYSTICK_VRX 34
#define JOYSTICK_VRY 35

void setup() {
  Serial.begin(115200);
  pinMode(JOYSTICK_VRX, INPUT);
  pinMode(JOYSTICK_VRY, INPUT);
}

void loop() {
  int xValue = analogRead(JOYSTICK_VRX);  // טווח: 0–4095
  int yValue = analogRead(JOYSTICK_VRY);  // טווח: 0–4095

  Serial.print("xValue: ");
  Serial.print(xValue);
  Serial.print(" | yValue: ");
  Serial.print(yValue);
  Serial.print(" | כיוון: ");

  if (xValue < 100 && yValue > 4000) {
    Serial.println("קדימה");
  } else if (xValue > 4000 && yValue > 4000) {
    Serial.println("אחורה");
  } else if (yValue < 1000) {
    Serial.println("ימינה");
  } else if (yValue > 3900 && xValue > 2900 && xValue < 4000) {
    Serial.println("שמאלה");
  } else {
    Serial.println("עצירה");
  }

  delay(300);
}
