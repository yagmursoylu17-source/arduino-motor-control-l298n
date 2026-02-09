/*
  Arduino UNO + L298N Motor Test
  Pins (senin önceki bağlantılarınla uyumlu):
  ENA=5, ENB=6, IN1=7, IN2=8, IN3=9, IN4=10

  Test: ileri -> dur -> geri -> sol dönüş -> sağ dönüş -> dur
*/

const int ENA = 5;
const int ENB = 6;
const int IN1 = 7;
const int IN2 = 8;
const int IN3 = 9;
const int IN4 = 10;

int basePWM = 170;   // 0-255 arası, motor gücüne göre ayarla
int turnPWM = 160;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();
}

void loop() {
  forward(basePWM);
  delay(1500);

  stopMotors();
  delay(500);

  backward(basePWM);
  delay(1500);

  stopMotors();
  delay(500);

  turnLeft(turnPWM);
  delay(900);

  stopMotors();
  delay(500);

  turnRight(turnPWM);
  delay(900);

  stopMotors();
  delay(1500);
}

// ---------------- Motor Fonksiyonları ----------------
void forward(int pwm) {
  // Sol motor ileri
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Sağ motor ileri
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
}

void backward(int pwm) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
}

void turnLeft(int pwm) {
  // Sol motor geri, sağ motor ileri
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
}

void turnRight(int pwm) {
  // Sol motor ileri, sağ motor geri
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, pwm);
  analogWrite(ENB, pwm);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
