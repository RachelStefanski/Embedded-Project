// === חיישן רטיבות ===
#define SOIL_SENSOR_PIN 36  // VP

// === אולטרסוניק קדמי ===
#define ULTRASONIC_FRONT_TRIG 14
#define ULTRASONIC_FRONT_ECHO 27

// === אולטרסוניק אחורי ===
#define ULTRASONIC_BACK_TRIG 12
#define ULTRASONIC_BACK_ECHO 13

// === אולטרסוניק שמאל ===
#define ULTRASONIC_LEFT_TRIG 32
#define ULTRASONIC_LEFT_ECHO 33

// === אולטרסוניק ימין ===
#define ULTRASONIC_RIGHT_TRIG 25
#define ULTRASONIC_RIGHT_ECHO 26

// === ג'ויסטיק ===
#define JOYSTICK_VRX 34
#define JOYSTICK_VRY 35
// טווחים לקליטה של הכיוונים
const int X_FORWARD_MAX = 100;       // x < 100 => קדימה
const int Y_FORWARD_MIN = 4000;      // y > 4000 => קדימה

const int X_BACKWARD_MIN = 4000;     // x > 4000 => אחורה
const int Y_BACKWARD_MIN = 4000;     // y > 4000 => אחורה

const int Y_RIGHT_MAX = 1000;        // y < 1000 => ימינה

const int Y_LEFT_MIN = 3900;         // y > 3900
const int X_LEFT_MIN = 2900;         // x בין 2900 ל-4000
const int X_LEFT_MAX = 4000;

// === MPU6050 (I2C) ===
#define MPU_SDA 21
#define MPU_SCL 22

// === דרייבר מנועים (Fun Motor) ===
#define MOTOR_RIGHT_INA 5     // ימני
#define MOTOR_RIGHT_INB 4
#define MOTOR_FRONT_INA 23    // קדמי
#define MOTOR_FRONT_INB 15

// === דרייבר משאבה ===
#define PUMP_RELAY 18
// #define PUMP_IN2 19

// === הגדרת מרחק להתנגשות===
#define COLLISION_THRESHOLD_CM 10

// === הגדרת סף לרטיבות===
#define DRY_THRESHOLD 2000  // מתחת לזה נחשב "יש רטיבות"


void setup() {
  Serial.begin(115200);

  // מנועים
  pinMode(MOTOR_RIGHT_INA, OUTPUT);
  pinMode(MOTOR_RIGHT_INB, OUTPUT);
  pinMode(MOTOR_FRONT_INA, OUTPUT);
  pinMode(MOTOR_FRONT_INB, OUTPUT);

  // משאבה
  pinMode(PUMP_RELAY, OUTPUT);

  // אולטרסוניק (TRIG כ-OUTPUT, ECHO כ-INPUT)
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
  
  // חיישן רטיבות
  pinMode(SOIL_SENSOR_PIN, INPUT);
}

// פונקציית מדידת מרחק
long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // timeout אחרי 30ms
  long distance = duration * 0.034 / 2; // מהירות קול באוויר: 0.034 ס"מ/μs

  if (duration == 0) return -1; // אם לא התקבלה החזרה, נחשב שגיאה
  return distance;
}

void loop() {
  long front = readDistanceCM(ULTRASONIC_FRONT_TRIG, ULTRASONIC_FRONT_ECHO);
  long back  = readDistanceCM(ULTRASONIC_BACK_TRIG, ULTRASONIC_BACK_ECHO);
  long left  = readDistanceCM(ULTRASONIC_LEFT_TRIG, ULTRASONIC_LEFT_ECHO);
  long right = readDistanceCM(ULTRASONIC_RIGHT_TRIG, ULTRASONIC_RIGHT_ECHO);

  Serial.println("=== בדיקת התנגשות ===");

  bool collision = false;

  if (front != -1 && front < COLLISION_THRESHOLD_CM) {
    Serial.print("⚠️  סכנת התנגשות מקדימה! מרחק: ");
    Serial.println(front);
    collision = true;
  }

  if (back != -1 && back < COLLISION_THRESHOLD_CM) {
    Serial.print("⚠️  סכנת התנגשות מאחור! מרחק: ");
    Serial.println(back);
    collision = true;
  }

  if (left != -1 && left < COLLISION_THRESHOLD_CM) {
    Serial.print("⚠️  סכנת התנגשות מצד שמאל! מרחק: ");
    Serial.println(left);
    collision = true;
  }

  if (right != -1 && right < COLLISION_THRESHOLD_CM) {
    Serial.print("⚠️  סכנת התנגשות מצד ימין! מרחק: ");
    Serial.println(right);
    collision = true;
  }

  if (!collision) {
    Serial.println("✔️ אין סכנת התנגשות");
  }

  Serial.println("=====================");
  delay(500);

// בדיקת רטיבות
    int soilValue = analogRead(SOIL_SENSOR_PIN);
  Serial.print("קריאת רטיבות: ");
  Serial.println(soilValue);

  if (soilValue < DRY_THRESHOLD) {
    Serial.println("✅ אין רטיבות — המשאבה כבויה");
    digitalWrite(PUMP_RELAY, LOW);  // לא צריך השקיה
  } else {
    Serial.println("💧 יש רטיבות — המשאבה פועלת!");
    digitalWrite(PUMP_RELAY, HIGH); // השקיה
  }

  delay(2000); // בדיקה כל 2 שניות

  int xValue = analogRead(JOYSTICK_VRX);
  int yValue = analogRead(JOYSTICK_VRY);

  Serial.print("xValue: ");
  Serial.print(xValue);
  Serial.print(" | yValue: ");
  Serial.print(yValue);
  Serial.print(" | כיוון: ");

  if (xValue < X_FORWARD_MAX && yValue > Y_FORWARD_MIN) {
    Serial.println("קדימה");
  } else if (xValue > X_BACKWARD_MIN && yValue > Y_BACKWARD_MIN) {
    Serial.println("אחורה");
  } else if (yValue < Y_RIGHT_MAX) {
    Serial.println("ימינה");
  } else if (yValue > Y_LEFT_MIN && xValue > X_LEFT_MIN && xValue < X_LEFT_MAX) {
    Serial.println("שמאלה");
  } else {
    Serial.println("עצירה");
  }

  delay(300);
}
