#define IN1 23
#define IN2 15
#define IN3 5
#define IN4 4

int stepIndex = 0;
const int steps[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1},
};

void stepMotor(int dir) {
  stepIndex = (stepIndex + dir + 8) % 8;
  digitalWrite(IN1, steps[stepIndex][0]);
  digitalWrite(IN2, steps[stepIndex][1]);
  digitalWrite(IN3, steps[stepIndex][2]);
  digitalWrite(IN4, steps[stepIndex][3]);
}

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
}

void loop() {
  Serial.println("Starting motor rotation..."); // הוסף את השורה הזו
  for (int i = 0; i < 512; i++) {
    stepMotor(1);
    delayMicroseconds(500);
  }
  Serial.println("Rotation complete."); // הוסף את השורה הזו
  while(true);
}