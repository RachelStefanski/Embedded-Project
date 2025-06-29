#include <TFT_eSPI.h>
#include <ESP32Servo.h>

// === תצוגת TFT ===
TFT_eSPI tft = TFT_eSPI(); // חיבור לתצוגה דרך SPI

// === חיישן רטיבות ===
const int moistureDigitalPin = 2;
const int moistureAnalogPin = 36;
bool isWet = false;

// === סרוו וג'ויסטיק ===
Servo myServo;
const int servoPin = 4;       // סרוו
const int joyXPin = 34;       // ג'ויסטיק X

// === חיישן מרחק אולטרסוניק ===
#define Trig 19
#define Echo 18

// === משאבה (L9110S) ===
const int pumpPin1 = 26;
const int pumpPin2 = 27;

void setup() {
  Serial.begin(115200);

  // TFT
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  // רטיבות
  pinMode(moistureDigitalPin, INPUT);

  // סרוו
  myServo.attach(servoPin);
  myServo.write(90); // למצב אמצע

  // משאבה
  pinMode(pumpPin1, OUTPUT);
  pinMode(pumpPin2, OUTPUT);
  digitalWrite(pumpPin1, LOW);
  digitalWrite(pumpPin2, LOW);

  // אולטרסוניק
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
}

float checkDistance() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  float duration = pulseIn(Echo, HIGH);
  return duration / 58.0;
}

void loop() {
  readSensors(); // ג'ויסטיק, אולטרסוניק, רטיבות, IMU

  if (isWet()) {
    stopMotors();
    activatePump();
    notifyMom("רטיבות זוהתה!");
    return;
  }

  if (isTippedOver()) {
    stopMotors();
    notifyMom("שיפוע חריג – סכנת התהפכות!");
    return;
  }

  if (isNearBoundary()) {
    restrictMovement(); // לא נאפשר תנועה לכיוון המסוכן
    notifyMom("קרבה לגבול");
  }

  if (hasMomCommand()) {
    applyMomCommand();
  } else {
    applyJoystickMovement();
  }

  updateDisplay();
  streamCamera(); // אופציונלי, אם לא רץ ברקע קבוע
  sendStatusToMom(); // WiFi
}

