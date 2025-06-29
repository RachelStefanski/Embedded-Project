#include <ESP32Servo.h> // ספרייה לשליטה בסרוו עם ESP32

// הגדרות עבור הסרוו
Servo myServo;        // יצירת אובייקט סרוו
const int servoPin = 2; // הפין שאליו חיברת את חוט האות של הסרוו (D2 / GPIO2)

// הגדרות עבור הג'ויסטיק
const int joyXPin = 34; // פין אנלוגי לציר ה-X של הג'ויסטיק (לדוגמה, GPIO34)
// const int joyYPin = 35; // פין אנלוגי לציר ה-Y של הג'ויסטיק (אופציונלי, אם משתמשים בשני סרווים או לשליטה אחרת)
// const int joyButtonPin = 15; // פין דיגיטלי לכפתור הג'ויסטיק (אופציונלי)

void setup() {
  Serial.begin(115200); // אתחול תקשורת טורית
  Serial.println("Joystick to Servo Control");

  // הגדרת פין הסרוו
  myServo.attach(servoPin);
  myServo.write(90); // הגדרת הסרוו למרכז בתחילה

  // הגדרת פיני הג'ויסטיק (אפשרי לקרוא אנלוגי ללא הגדרה ב-pinMode)
  // pinMode(joyButtonPin, INPUT_PULLUP); // אם משתמשים בכפתור

  Serial.println("Setup complete. Move joystick to control servo.");
}

void loop() {
  // קריאת הערך מציר ה-X של הג'ויסטיק
  int joyXValue = analogRead(joyXPin);
  // Serial.print("Joystick X Value: ");
  // Serial.println(joyXValue);

  // המרת ערך הג'ויסטיק (0-4095) לזווית סרוו (0-180)
  // ייתכן שתצטרך לכוונן את טווח המינימום/מקסימום של הג'ויסטיק
  // כדי להתאים אותו בדיוק למודול הספציפי שלך.
  // למשל, אם הג'ויסטיק לא מגיע ממש ל-0 או 4095, שנה את הטווח כאן.
  int servoAngle = map(joyXValue, 0, 4095, 0, 180);

  // הגבלת הזווית לוודא שהיא בתוך הטווח 0-180 (למקרה שה-map יפיק משהו חריג)
  servoAngle = constrain(servoAngle, 0, 180);

  // כתיבת הזווית לסרוו
  myServo.write(servoAngle);

  // הדפסה ל-Serial Monitor (אפשר להפעיל/לבטל את ההערות)
  // Serial.print("Mapped Servo Angle: ");
  // Serial.println(servoAngle);

  // עיכוב קצר כדי למנוע קריאה מהירה מדי
  delay(10);
}