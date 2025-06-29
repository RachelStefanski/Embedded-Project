#include <WiFi.h>
#include <WebServer.h>
// #include <ESP32Servo.h> // בטל הערה אם משתמשים במנוע סרוו

// ===== הגדרות Wi-Fi =====
const char* ssid = "RutiTikochinski";     // <-- שנה לשם רשת ה-Wi-Fi שלך
const char* password = "325672657"; // <-- שנה לסיסמת רשת ה-Wi-Fi שלך

// ===== הגדרות פינים למנועים (DC Motors עם דרייבר H-bridge) =====
// מנוע ראשי (Fan קדמי)
#define FAN_MAIN_INA 23 // Fan קדמי INA - ודא פין נכון (לפי הטבלה שלך, זה D5)
#define FAN_MAIN_INB 15 // Fan קדמי INB - ודא פין נכון (לפי הטבלה שלך, זה D4)
#define FAN_MAIN_EN 5   // Fan קדמי EN (מהירות PWM)

// מנוע ימין (Fan ימין - מנוע כיוון) - **הערה: אם תשתמשי בסרוו להיגוי, תצטרכי לשנות את החלק הזה**
#define FAN_RIGHT_INA 4   // Fan ימין INA - ודא פין נכון (לפי הטבלה שלך, זה D23)
#define FAN_RIGHT_INB 18  // Fan ימין INB - ודא פין נכון (לפי הטבלה שלך, זה D15)
#define FAN_RIGHT_EN 19   // Fan ימין EN (מהירות PWM)

// הגדרת מהירות מנועים
const int motorSpeed = 150; // טווח 0-255 עבור analogWrite (PWM)

// ===== אובייקט שרת ווב =====
WebServer server(80); // השרת יאזין בפורט 80 (HTTP סטנדרטי)

// ===== פונקציות עזר לשליטה במנועי DC (מהקוד המקורי שלך) =====
void moveMotor(int INA, int INB, int EN_PIN, int speedValue) {
  // speedValue חיובי לקדימה, שלילי לאחורה, 0 לעצירה
  if (speedValue > 0) {
    digitalWrite(INA, HIGH);
    digitalWrite(INB, LOW);
    analogWrite(EN_PIN, speedValue);
  } else if (speedValue < 0) {
    digitalWrite(INA, LOW);
    digitalWrite(INB, HIGH);
    analogWrite(EN_PIN, -speedValue); // מהירות חיובית
  } else {
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    analogWrite(EN_PIN, 0); // עצירה
  }
}

void moveForward() {
  Serial.println("Moving Forward");
  moveMotor(FAN_MAIN_INA, FAN_MAIN_INB, FAN_MAIN_EN, motorSpeed);
  // במקרה של 2 מנועים לדחיפה קדימה:
  // moveMotor(FAN_RIGHT_INA, FAN_RIGHT_INB, FAN_RIGHT_EN, motorSpeed);
}

void moveBackward() {
  Serial.println("Moving Backward");
  moveMotor(FAN_MAIN_INA, FAN_MAIN_INB, FAN_MAIN_EN, -motorSpeed);
  // במקרה של 2 מנועים לדחיפה אחורה:
  // moveMotor(FAN_RIGHT_INA, FAN_RIGHT_INB, FAN_RIGHT_EN, -motorSpeed);
}

void turnLeft() {
  Serial.println("Turning Left");
  // כדי לפנות שמאלה, המנוע הראשי יזוז קדימה (או יעצור), והמנוע הימני יסתובב אחורה (או יעצור).
  // זה תלוי איך את רוצה שהסירה תסתובב.
  // דוגמה: מנוע ראשי קדימה, מנוע ימני אחורה (סיבוב על ציר)
  moveMotor(FAN_MAIN_INA, FAN_MAIN_INB, FAN_MAIN_EN, motorSpeed); // ראשי קדימה
  moveMotor(FAN_RIGHT_INA, FAN_RIGHT_INB, FAN_RIGHT_EN, -motorSpeed); // ימני אחורה
}

void turnRight() {
  Serial.println("Turning Right");
  // כדי לפנות ימינה, המנוע הראשי יזוז קדימה (או יעצור), והמנוע הימני יסתובב קדימה (או יעצור).
  // דוגמה: מנוע ראשי קדימה, מנוע ימני קדימה (פנייה רחבה יותר, או תלוי בסידור המנועים)
  moveMotor(FAN_MAIN_INA, FAN_MAIN_INB, FAN_MAIN_EN, motorSpeed); // ראשי קדימה
  moveMotor(FAN_RIGHT_INA, FAN_RIGHT_INB, FAN_RIGHT_EN, motorSpeed); // ימני קדימה
}

void stopAllMotors() {
  Serial.println("Stopping All Motors");
  moveMotor(FAN_MAIN_INA, FAN_MAIN_INB, FAN_MAIN_EN, 0); // עצור מנוע ראשי
  moveMotor(FAN_RIGHT_INA, FAN_RIGHT_INB, FAN_RIGHT_EN, 0); // עצור מנוע ימני
}

// ===== פונקציית טיפול בבקשות HTTP =====
void handleControl() {
  String cmd = server.arg("cmd"); // קבל את הפרמטר 'cmd' מה-URL
  Serial.print("Received command: ");
  Serial.println(cmd);

  if (cmd == "forward") {
    moveForward();
  } else if (cmd == "backward") {
    moveBackward();
  } else if (cmd == "left") {
    turnLeft();
  } else if (cmd == "right") {
    turnRight();
  } else if (cmd == "stop") {
    stopAllMotors();
  } else {
    Serial.println("Unknown command");
  }

  server.send(200, "text/plain", "OK"); // שלח תגובה לאפליקציה
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);

  // הגדרת פיני המנועים כפלט
  pinMode(FAN_MAIN_INA, OUTPUT);
  pinMode(FAN_MAIN_INB, OUTPUT);
  pinMode(FAN_MAIN_EN, OUTPUT);
  pinMode(FAN_RIGHT_INA, OUTPUT);
  pinMode(FAN_RIGHT_INB, OUTPUT);
  pinMode(FAN_RIGHT_EN, OUTPUT);

  // וודא שהמנועים כבויים בהתחלה
  stopAllMotors();

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // התחבר לרשת ה-Wi-Fi

  // המתן עד לחיבור לרשת
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // הדפס את כתובת ה-IP של ה-ESP32

  // הגדר את הטיפול בבקשות HTTP
  server.on("/control", handleControl); // כאשר מגיעה בקשה לנתיב /control, קרא לפונקציה handleControl

  // התחל את השרת
  server.begin();
  Serial.println("Web server started.");
}

// ===== Loop =====
void loop() {
  server.handleClient(); // טפל בבקשות HTTP נכנסות
}