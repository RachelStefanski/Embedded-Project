#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <WiFi.h>
#include <WebServer.h>

// ==== הגדרות פינים =====
#define SOIL_SENSOR_PIN 36           // חיישן רטיבות
#define JOYSTICK_VRX 34              // ג'ויסטיק ציר X
#define JOYSTICK_VRY 35              // ג'ויסטיק ציר Y
#define ULTRASONIC_FRONT_TRIG 14    // אולטרסוניק קדמי - TRIG
#define ULTRASONIC_FRONT_ECHO 27    // אולטרסוניק קדמי - ECHO
#define ULTRASONIC_BACK_TRIG 12     // אולטרסוניק אחורי - TRIG
#define ULTRASONIC_BACK_ECHO 13     // אולטרסוניק אחורי - ECHO
#define ULTRASONIC_LEFT_TRIG 32     // אולטרסוניק שמאל - TRIG
#define ULTRASONIC_LEFT_ECHO 33     // אולטרסוניק שמאל - ECHO
#define ULTRASONIC_RIGHT_TRIG 25    // אולטרסוניק ימין - TRIG
#define ULTRASONIC_RIGHT_ECHO 26    // אולטרסוניק ימין - ECHO
#define MOTOR_RIGHT_INA 5           // מנוע ימין - כיוון 1
#define MOTOR_RIGHT_INB 4           // מנוע ימין - כיוון 2
#define MOTOR_FRONT_INA 23          // מנוע קדמי - כיוון 1
#define MOTOR_FRONT_INB 15          // מנוע קדמי - כיוון 2
#define PUMP_RELAY 18               // ריליי למשאבה
#define MPU_SDA 21                  // MPU6050 - SDA
#define MPU_SCL 22                  // MPU6050 - SCL

// ==== קבועים ==== 
const int COLLISION_THRESHOLD_CM = 10;    // סף התנגשות באולטרסוניק
const int DRY_THRESHOLD = 2000;           // סף רטיבות
const float accelThreshold = 2.5;         // סף תאוצה מסוכנת
const float directionalAccelThreshold = 1.0; // תאוצה חזקה בכיוון אחד
const float flipThreshold = 0.7;          // זווית התהפכות

// ==== רשת ==== 
WebServer server(80);                     // שרת HTTP

// const char* ssid = "HUAWEI-9E9A";
// const char* password = "036616258";

const char* ssid = "Kita-2";
const char* password = "Xnhbrrfxho";

// ==== MPU ==== 
MPU9250_asukiaaa mySensor;
float baseX = 0, baseY = 0, baseZ = 0;    // כיול מצב יציב

// ==== setup ==== 
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);            // התחברות לרשת
  while (WiFi.status() != WL_CONNECTED) delay(500);
  server.on("/control", handleControl);  // פקודות מהאפליקציה
  server.begin();

  // יציאות
  pinMode(PUMP_RELAY, OUTPUT);
  pinMode(MOTOR_RIGHT_INA, OUTPUT);
  pinMode(MOTOR_RIGHT_INB, OUTPUT);
  pinMode(MOTOR_FRONT_INA, OUTPUT);
  pinMode(MOTOR_FRONT_INB, OUTPUT);

  // אולטרסוניק
  pinMode(ULTRASONIC_FRONT_TRIG, OUTPUT);
  pinMode(ULTRASONIC_FRONT_ECHO, INPUT);
  pinMode(ULTRASONIC_BACK_TRIG, OUTPUT);
  pinMode(ULTRASONIC_BACK_ECHO, INPUT);
  pinMode(ULTRASONIC_LEFT_TRIG, OUTPUT);
  pinMode(ULTRASONIC_LEFT_ECHO, INPUT);
  pinMode(ULTRASONIC_RIGHT_TRIG, OUTPUT);
  pinMode(ULTRASONIC_RIGHT_ECHO, INPUT);

  // ג'ויסטיק
  pinMode(JOYSTICK_VRX, INPUT);
  pinMode(JOYSTICK_VRY, INPUT);

  // MPU6050
  Wire.begin(MPU_SDA, MPU_SCL);
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  calibrateBaseline();                   // הכיול הראשוני
}

// ==== טיפול בבקשת שליטה מהאפליקציה ====
String lastCommand = "stop";
void handleControl() {
  lastCommand = server.arg("cmd");
  server.send(200, "text/plain", "OK");
}

// ==== פונקציית קריאת מרחק ====
long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  return (duration == 0) ? -1 : duration * 0.034 / 2;
}

// ==== כיול ראשוני למצב יציב ====
void calibrateBaseline() {
  for (int i = 0; i < 100; i++) {
    mySensor.accelUpdate();
    baseX += mySensor.accelX();
    baseY += mySensor.accelY();
    baseZ += mySensor.accelZ();
    delay(10);
  }
  baseX /= 100; baseY /= 100; baseZ /= 100;
}

// ==== שלב בטיחות - החזר true אם יש סכנה ====
bool safetyControl() {
  long f = readDistanceCM(ULTRASONIC_FRONT_TRIG, ULTRASONIC_FRONT_ECHO);
  long b = readDistanceCM(ULTRASONIC_BACK_TRIG, ULTRASONIC_BACK_ECHO);
  long l = readDistanceCM(ULTRASONIC_LEFT_TRIG, ULTRASONIC_LEFT_ECHO);
  long r = readDistanceCM(ULTRASONIC_RIGHT_TRIG, ULTRASONIC_RIGHT_ECHO);

  // אם יש סכנת התנגשות, הפעל את המנועים בכיוון ההפוך
  if ((f!=-1 && f<COLLISION_THRESHOLD_CM)) {
    Serial.println("⚠️ סכנת התנגשות קדימה - סיבוב אחורה");
    moveBackward();
    return true;
  }
  else if ((b!=-1 && b<COLLISION_THRESHOLD_CM)) {
    Serial.println("⚠️ סכנת התנגשות אחורה - סיבוב קדימה");
    moveForward();
    return true;
  }
  else if ((l!=-1 && l<COLLISION_THRESHOLD_CM)) {
    Serial.println("⚠️ סכנת התנגשות שמאלה - סיבוב ימינה");
    turnRight();
    return true;
  }
  else if ((r!=-1 && r<COLLISION_THRESHOLD_CM)) {
    Serial.println("⚠️ סכנת התנגשות ימינה - סיבוב שמאלה");
    turnLeft();
    return true;
  }

  mySensor.accelUpdate();
  float ax = mySensor.accelX(), ay = mySensor.accelY(), az = mySensor.accelZ();
  float totalAccel = sqrt(ax * ax + ay * ay + az * az);

  // אם תאוצה מופרזת, הפעל את המנועים בכיוון ההפוך
  if (totalAccel > accelThreshold) {
    Serial.println("⚠️ תאוצה מופרזת - סיבוב בכיוון ההפוך");
    moveBackward();
    return true;
  }

  float dot = ax*baseX + ay*baseY + az*baseZ;
  float magC = sqrt(ax*ax + ay*ay + az*az);
  float magB = sqrt(baseX*baseX + baseY*baseY + baseZ*baseZ);
  float angle = acos(dot / (magC * magB));

  // אם זווית התהפכות, עצור את המנועים
  if (angle > flipThreshold) {
    Serial.println("⚠️ התהפכות - עצירה");
    stopMotors();
    return true;
  }

  return false;
}

// ==== שליטה מהאם ====
bool motherControl() {
  server.handleClient();
  if (lastCommand == "forward") {
    Serial.println("⬆️ אמא: קדימה");
    moveForward();
  }
  else if (lastCommand == "back") {
    Serial.println("⬇️ אמא: אחורה");
    moveBackward();
  }
  else if (lastCommand == "left") {
    Serial.println("⬅️ אמא: שמאלה");
    turnLeft();
  }
  else if (lastCommand == "right") {
    Serial.println("➡️ אמא: ימינה");
    turnRight();
  }
  else {
    Serial.println("⏹️ אמא: עצירה");
    stopMotors();
  }
  return lastCommand != "stop";
}

// ==== שליטה מג'ויסטיק ====
void joystickControl() {
  int x = analogRead(JOYSTICK_VRX);
  int y = analogRead(JOYSTICK_VRY);
  if (x < 100 && y > 4000) {
    Serial.println("⬆️ ג'ויסטיק: קדימה");
    moveForward();
  }
  else if (x > 4000 && y > 4000) {
    Serial.println("⬇️ ג'ויסטיק: אחורה");
    moveBackward();
  }
  else if (y < 1000) {
    Serial.println("➡️ ג'ויסטיק: ימינה");
    turnRight();
  }
  else if (y > 3900 && x > 2900 && x < 4000) {
    Serial.println("⬅️ ג'ויסטיק: שמאלה");
    turnLeft();
  }
  else {
    Serial.println("⏹️ ג'ויסטיק: עצירה");
    stopMotors();
  }
}

// ==== בדיקת רטיבות ====
void checkSoil() {
  int value = analogRead(SOIL_SENSOR_PIN);
  if (value < DRY_THRESHOLD) {
    Serial.println("✅ יבש - משאבה כבויה");
    digitalWrite(PUMP_RELAY, LOW);
  } else {
    Serial.println("💧 רטוב - משאבה פועלת");
    digitalWrite(PUMP_RELAY, HIGH);
  }
}

// ==== הפעלת מנועים ====
void stopMotors() {
  digitalWrite(MOTOR_FRONT_INA, LOW);
  digitalWrite(MOTOR_FRONT_INB, LOW);
  digitalWrite(MOTOR_RIGHT_INA, LOW);
  digitalWrite(MOTOR_RIGHT_INB, LOW);
}

void moveForward() {
  digitalWrite(MOTOR_FRONT_INA, HIGH);
  digitalWrite(MOTOR_FRONT_INB, LOW);
}

void moveBackward() {
  digitalWrite(MOTOR_FRONT_INA, LOW);
  digitalWrite(MOTOR_FRONT_INB, HIGH);
}

void turnRight() {
  digitalWrite(MOTOR_RIGHT_INA, HIGH);
  digitalWrite(MOTOR_RIGHT_INB, LOW);
}

void turnLeft() {
  digitalWrite(MOTOR_RIGHT_INA, LOW);
  digitalWrite(MOTOR_RIGHT_INB, HIGH);
}

// ==== loop ==== 
void loop() {
  if (safetyControl()) return;     // בטיחות גוברת על הכול
  if (motherControl()) return;     // שליטת אם
  joystickControl();               // ג'ויסטיק
  checkSoil();                     // בדיקת רטיבות
  delay(500);
}
