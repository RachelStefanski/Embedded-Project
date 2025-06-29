#define RELAY_PIN 25

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // מכובה בתחילה
  Serial.begin(115200);
}

void loop() {
  digitalWrite(RELAY_PIN, HIGH); // הפעלת המשאבה
}
