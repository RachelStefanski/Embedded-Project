// #define FAN_RIGHT_INA 5
// #define FAN_RIGHT_INB 4
// #define FAN_FRONT_INA 23
// #define FAN_FRONT_INB 15

// void setup() {
//   // הגדרת הפינים כמוצא
//   pinMode(FAN_FRONT_INA, OUTPUT);
//   pinMode(FAN_FRONT_INB, OUTPUT);
//   pinMode(FAN_RIGHT_INA, OUTPUT);
//   pinMode(FAN_RIGHT_INB, OUTPUT);

//   Serial.begin(115200);
// }

// // פונקציות עזר
// void stopMotors() {
//   digitalWrite(FAN_FRONT_INA, LOW);
//   digitalWrite(FAN_FRONT_INB, LOW);
//   digitalWrite(FAN_RIGHT_INA, LOW);
//   digitalWrite(FAN_RIGHT_INB, LOW);
// }

// void moveForward() {
//   digitalWrite(FAN_FRONT_INA, HIGH);
//   digitalWrite(FAN_FRONT_INB, LOW);
//   Serial.println("Forward");
// }

// void moveBackward() {
//   digitalWrite(FAN_FRONT_INA, LOW);
//   digitalWrite(FAN_FRONT_INB, HIGH);
//   Serial.println("Backward");
// }

// void turnRight() {
//   digitalWrite(FAN_RIGHT_INA, HIGH);
//   digitalWrite(FAN_RIGHT_INB, LOW);
//   Serial.println("Right");
// }

// void turnLeft() {
//   digitalWrite(FAN_RIGHT_INA, LOW);
//   digitalWrite(FAN_RIGHT_INB, HIGH);
//   Serial.println("Left");
// }

// void loop() {
//   moveForward();
//   delay(1000);

//   moveBackward();
//   delay(1000);

//   turnRight();
//   delay(1000);

//   turnLeft();
//   delay(1000);

//   stopMotors();
//   delay(1000);
// }

#define FAN_RIGHT_INA 15
#define FAN_RIGHT_INB 23

// #define FAN_RIGHT_INA 16
// #define FAN_RIGHT_INB 17

void setup() {
  // הגדרת הפינים כמוצא
  pinMode(FAN_RIGHT_INA, OUTPUT);
  pinMode(FAN_RIGHT_INB, OUTPUT);
  Serial.begin(115200);
}

void stopRightMotor() {
  digitalWrite(FAN_RIGHT_INA, LOW);
  digitalWrite(FAN_RIGHT_INB, LOW);
}

void turnRight() {
  digitalWrite(FAN_RIGHT_INA, HIGH);
  digitalWrite(FAN_RIGHT_INB, LOW);
  Serial.println("Right");
}

void turnLeft() {
  digitalWrite(FAN_RIGHT_INA, LOW);
  digitalWrite(FAN_RIGHT_INB, HIGH);
  Serial.println("Left");
}

void loop() {
  turnRight();
  delay(1000);

  turnLeft();
  delay(1000);

  stopRightMotor();
  delay(1000);
}
