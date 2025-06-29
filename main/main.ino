// // === חיישן רטיבות ===
// #define SOIL_SENSOR_PIN 36  // VP

// // === אולטרסוניק קדמי ===
// #define ULTRASONIC_FRONT_TRIG 14
// #define ULTRASONIC_FRONT_ECHO 27

// // === אולטרסוניק אחורי ===
// #define ULTRASONIC_BACK_TRIG 12
// #define ULTRASONIC_BACK_ECHO 13

// // === אולטרסוניק שמאל ===
// #define ULTRASONIC_LEFT_TRIG 32
// #define ULTRASONIC_LEFT_ECHO 33

// // === אולטרסוניק ימין ===
// #define ULTRASONIC_RIGHT_TRIG 25
// #define ULTRASONIC_RIGHT_ECHO 26

// // === ג'ויסטיק ===
// #define JOYSTICK_VRX 34
// #define JOYSTICK_VRY 35

// // === MPU6050 (I2C) ===
// #define MPU_SDA 21
// #define MPU_SCL 22

// // === דרייבר מנועים (Fun Motor) ===
// #define MOTOR_RIGHT_INA 5     // ימני
// #define MOTOR_RIGHT_INB 4
// #define MOTOR_FRONT_INA 23    // קדמי
// #define MOTOR_FRONT_INB 15

// // === דרייבר משאבה ===
// #define PUMP_RELAY 18
// // #define PUMP_IN2 19

// // === הגדרת מרחק להתנגשות===
// #define COLLISION_THRESHOLD_CM 10

// // === הגדרת סף לרטיבות===
// #define DRY_THRESHOLD 2000  // מתחת לזה נחשב "יש רטיבות"

// // === הגדרת סטיית ג'ויסטיק===
// // #define DEAD_ZONE 200  // טווח סטייה מותר סביב המרכז

// #define DEAD_ZONE 10  // רגישות מתונה
// // #define centerX 0
// // #define centerY 0
// int centerX = 0;
// int centerY = 0;


// void setup() {
//   Serial.begin(115200);

//   // מנועים
//   pinMode(MOTOR_RIGHT_INA, OUTPUT);
//   pinMode(MOTOR_RIGHT_INB, OUTPUT);
//   pinMode(MOTOR_FRONT_INA, OUTPUT);
//   pinMode(MOTOR_FRONT_INB, OUTPUT);

//   // משאבה
//   pinMode(PUMP_RELAY, OUTPUT);

//   // אולטרסוניק (TRIG כ-OUTPUT, ECHO כ-INPUT)
//   pinMode(ULTRASONIC_FRONT_TRIG, OUTPUT);
//   pinMode(ULTRASONIC_FRONT_ECHO, INPUT);
//   pinMode(ULTRASONIC_BACK_TRIG, OUTPUT);
//   pinMode(ULTRASONIC_BACK_ECHO, INPUT);
//   pinMode(ULTRASONIC_LEFT_TRIG, OUTPUT);
//   pinMode(ULTRASONIC_LEFT_ECHO, INPUT);
//   pinMode(ULTRASONIC_RIGHT_TRIG, OUTPUT);
//   pinMode(ULTRASONIC_RIGHT_ECHO, INPUT);

//   // ג'ויסטיק
//   pinMode(JOYSTICK_VRX, INPUT);
//   pinMode(JOYSTICK_VRY, INPUT);
//   calibrateJoystick();

//   // חיישן רטיבות
//   pinMode(SOIL_SENSOR_PIN, INPUT);
// }

// // פונקציית מדידת מרחק
// long readDistanceCM(int trigPin, int echoPin) {
//   digitalWrite(trigPin, LOW);
//   delayMicroseconds(2);
//   digitalWrite(trigPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigPin, LOW);

//   long duration = pulseIn(echoPin, HIGH, 30000);  // timeout אחרי 30ms
//   long distance = duration * 0.034 / 2; // מהירות קול באוויר: 0.034 ס"מ/μs

//   if (duration == 0) return -1; // אם לא התקבלה החזרה, נחשב שגיאה
//   return distance;
// }

// // פונקציית חישוב מרכז
// void calibrateJoystick() {
//   long sumX = 0;
//   long sumY = 0;
//   for (int i = 0; i < 10; i++) {
//     sumX += analogRead(JOYSTICK_VRX);
//     sumY += analogRead(JOYSTICK_VRY);
//     delay(100);
//   }
//   centerX = sumX / 10;
//   centerY = sumY / 10;
//   Serial.print("CenterX: "); Serial.println(centerX);
//   Serial.print("CenterY: "); Serial.println(centerY);
// }

// void loop() {
//   long front = readDistanceCM(ULTRASONIC_FRONT_TRIG, ULTRASONIC_FRONT_ECHO);
//   long back  = readDistanceCM(ULTRASONIC_BACK_TRIG, ULTRASONIC_BACK_ECHO);
//   long left  = readDistanceCM(ULTRASONIC_LEFT_TRIG, ULTRASONIC_LEFT_ECHO);
//   long right = readDistanceCM(ULTRASONIC_RIGHT_TRIG, ULTRASONIC_RIGHT_ECHO);

//   Serial.println("=== בדיקת התנגשות ===");

//   bool collision = false;

//   if (front != -1 && front < COLLISION_THRESHOLD_CM) {
//     Serial.print("⚠️  סכנת התנגשות מקדימה! מרחק: ");
//     Serial.println(front);
//     collision = true;
//   }

//   if (back != -1 && back < COLLISION_THRESHOLD_CM) {
//     Serial.print("⚠️  סכנת התנגשות מאחור! מרחק: ");
//     Serial.println(back);
//     collision = true;
//   }

//   if (left != -1 && left < COLLISION_THRESHOLD_CM) {
//     Serial.print("⚠️  סכנת התנגשות מצד שמאל! מרחק: ");
//     Serial.println(left);
//     collision = true;
//   }

//   if (right != -1 && right < COLLISION_THRESHOLD_CM) {
//     Serial.print("⚠️  סכנת התנגשות מצד ימין! מרחק: ");
//     Serial.println(right);
//     collision = true;
//   }

//   if (!collision) {
//     Serial.println("✔️ אין סכנת התנגשות");
//   }

//   Serial.println("=====================");
//   delay(500);

// // בדיקת רטיבות
//     int soilValue = analogRead(SOIL_SENSOR_PIN);
//   Serial.print("קריאת רטיבות: ");
//   Serial.println(soilValue);

//   if (soilValue < DRY_THRESHOLD) {
//     Serial.println("✅ אין רטיבות — המשאבה כבויה");
//     digitalWrite(PUMP_RELAY, LOW);  // לא צריך השקיה
//   } else {
//     Serial.println("💧 יש רטיבות — המשאבה פועלת!");
//     digitalWrite(PUMP_RELAY, HIGH); // השקיה
//   }

//   delay(2000); // בדיקה כל 2 שניות

// // ג'ויסטיק
//   int xValue = analogRead(JOYSTICK_VRX);
//   int yValue = analogRead(JOYSTICK_VRY);

//   bool moved = false;

//   Serial.print("כיוון: ");
// Serial.print("CenterX: "); Serial.println(centerX);
// Serial.print("CenterY: "); Serial.println(centerY);

//   // ציר X – ימין/שמאל
//   if (xValue < centerX - DEAD_ZONE) {
//     Serial.print("שמאלה ");
//     moved = true;
//   } else if (xValue > centerX + DEAD_ZONE) {
//     Serial.print("ימינה ");
//     moved = true;
//   }

//   // ציר Y – קדימה/אחורה
//   if (yValue < centerY - DEAD_ZONE) {
//     Serial.print("אחורה ");
//     moved = true;
//   } else if (yValue > centerY + DEAD_ZONE) {
//     Serial.print("קדימה ");
//     moved = true;
//   }

//   if (!moved) {
//     Serial.print("עצירה");
//   }

//   Serial.println();
//   delay(300);
// }


#define JOYSTICK_VRX 34
#define JOYSTICK_VRY 35
#define DEAD_ZONE 50  // הגדל כדי למנוע רעשים

int centerX = 0;
int centerY = 0;

void calibrateJoystick() {
  long sumX = 0;
  long sumY = 0;
  for (int i = 0; i < 10; i++) {
    sumX += analogRead(JOYSTICK_VRX);
    sumY += analogRead(JOYSTICK_VRY);
    delay(50);
  }
  centerX = sumX / 10;
  centerY = sumY / 10;
  Serial.print("CenterX: "); Serial.println(centerX);
  Serial.print("CenterY: "); Serial.println(centerY);
}

void setup() {
  Serial.begin(115200);
  pinMode(JOYSTICK_VRX, INPUT);
  pinMode(JOYSTICK_VRY, INPUT);
  calibrateJoystick();
}

void loop() {
  int xValue = analogRead(JOYSTICK_VRX);
  int yValue = analogRead(JOYSTICK_VRY);

  Serial.print("xValue: ");
  Serial.print(xValue);
  Serial.print(" | yValue: ");
  Serial.print(yValue);
  Serial.print(" | כיוון: ");

  bool moved = false;

  if (xValue < centerX - DEAD_ZONE) {
    Serial.print("שמאלה ");
    moved = true;
  } else if (xValue > centerX + DEAD_ZONE) {
    Serial.print("ימינה ");
    moved = true;
  }

  // שימי לב: הפכתי את הלוגיקה בציר Y כי לפעמים הכיוון הפוך
  if (yValue > centerY + DEAD_ZONE) {
    Serial.print("קדימה ");
    moved = true;
  } else if (yValue < centerY - DEAD_ZONE) {
    Serial.print("אחורה ");
    moved = true;
  }

  if (!moved) {
    Serial.print("עצירה");
  }
  Serial.println();

  delay(200);
}
