

#define Trig 19
#define Echo 18


void setup() {
  Serial.begin(115200);
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
  float distance = duration / 58.0;
  return distance;
}


void loop() {
  float d = checkDistance();
  Serial.print("Distance: ");
  Serial.print(d);
  Serial.println(" cm");
  delay(200);
}
