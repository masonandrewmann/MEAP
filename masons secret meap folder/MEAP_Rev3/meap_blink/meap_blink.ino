void setup() {
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(36, HIGH);
  digitalWrite(37, LOW);
  delay(400);
  digitalWrite(36, LOW);
  digitalWrite(37, HIGH);
  delay(400);
}
