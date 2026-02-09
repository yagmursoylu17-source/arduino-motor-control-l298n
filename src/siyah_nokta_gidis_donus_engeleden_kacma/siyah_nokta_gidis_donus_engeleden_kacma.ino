/*
  MOTOR PINLERI (SENIN VERDIGİN):
  ENA 5, ENB 6, IN1 7, IN2 8, IN3 9, IN4 10  (L298N)

  LINE / SIYAH SENSOR (analog varsayım):
  SOL = A1, SAG = A0

  HC-SR04:
  TRIG=12, ECHO=11

  MANTIK:
  - Arena beyaz, hedef siyah daire.
  - Robot gezer (search). Siyahı görünce merkeze alır (approach).
  - Siyah dairenin üstünde (iki sensör de siyah) durur.
  - Geldiği hareketleri kaydettiği için aynı yolu TERSTEN oynatıp başlangıca (park) döner.
  - Önde duvar/engel görünce kaçış manevrası yapar (o da kayda girer).
*/

// ---------------- PINLER ----------------
const int ENA = 5;
const int ENB = 6;
const int IN1 = 7;
const int IN2 = 8;
const int IN3 = 9;
const int IN4 = 10;

const int LINE_L = A1;
const int LINE_R = A0;

const int TRIG_PIN = 12;
const int ECHO_PIN = 11;

// ------------- AYARLAR (SENİN DEĞERLERİNE UYGUN) -------------
int baseSpeed = 160;     // ileri hız
int steerGain = 50;      // siyaha yaklaşma düzeltmesi
int backSpeed = 140;     // kaçış geri hız
int turnSpeed = 170;     // pivot dönüş

// Motor düz gitmiyorsa burayı oyna:
int trimLeft  = 0;
int trimRight = 0;

// Motor yönü tersse true yap:
const bool INV_LEFT  = false;
const bool INV_RIGHT = false;

// Siyah/Beyaz eşiği (senin değerlerde beyaz 20-40, siyah 800-1000)
int THRESH_BLACK = 600;

// Engel eşiği (cm)
const int OBSTACLE_CM = 18;

// --------- Hareket Kaydı (geri dönüş için) ----------
struct Segment {
  int16_t l;
  int16_t r;
  uint16_t ms;
};

const int MAX_SEG = 120;
Segment seg[MAX_SEG];
int segCount = 0;

int16_t curL = 0, curR = 0;
unsigned long cmdStartMs = 0;

enum State { SEARCH, APPROACH, TARGET_FOUND, RETURNING, DONE };
State state = SEARCH;

int retIndex = -1;
unsigned long retStartMs = 0;

// ---------------- MOTOR ----------------
void setMotorRaw(int16_t l, int16_t r) {
  l = constrain(l, -255, 255);
  r = constrain(r, -255, 255);

  // invert opsiyonları
  if (INV_LEFT)  l = -l;
  if (INV_RIGHT) r = -r;

  // trim
  l = constrain((int)l + trimLeft,  -255, 255);
  r = constrain((int)r + trimRight, -255, 255);

  // Sol
  if (l >= 0) { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  }
  else        { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); }

  // Sağ
  if (r >= 0) { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  }
  else        { digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); }

  analogWrite(ENA, abs(l));
  analogWrite(ENB, abs(r));
}

void motorStop() {
  setMotorRaw(0, 0);
}

// segment ekle
void pushSeg(int16_t l, int16_t r, uint16_t ms) {
  if (ms < 25) return;
  if (segCount >= MAX_SEG) return; // dolarsa daha fazla kaydetmez (ama yine çalışır)
  seg[segCount++] = { l, r, ms };
}

// logging’li sürüş
void drive(int16_t l, int16_t r, bool logging=true) {
  if (!logging) {
    curL = l; curR = r;
    setMotorRaw(l, r);
    return;
  }

  unsigned long now = millis();
  if (cmdStartMs == 0) {
    cmdStartMs = now;
    curL = l; curR = r;
    setMotorRaw(l, r);
    return;
  }

  if (l == curL && r == curR) {
    // aynı komut devam
    setMotorRaw(l, r);
    return;
  }

  // komut değişti -> önceki segmenti kaydet
  unsigned long dt = now - cmdStartMs;
  pushSeg(curL, curR, (uint16_t)min(dt, 65535UL));

  // yeni komut
  cmdStartMs = now;
  curL = l; curR = r;
  setMotorRaw(l, r);
}

void finalizeLog() {
  if (cmdStartMs == 0) return;
  unsigned long now = millis();
  unsigned long dt = now - cmdStartMs;
  pushSeg(curL, curR, (uint16_t)min(dt, 65535UL));
  cmdStartMs = 0;
}

// ---------------- LINE SENSOR ----------------
int readLineAvg(int pin) {
  long s = 0;
  for (int i=0;i<5;i++){
    s += analogRead(pin);
    delayMicroseconds(600);
  }
  return (int)(s/5);
}

bool isBlack(int v) {
  // siyah yüksek okuyor varsayımı
  return v >= THRESH_BLACK;
}

// ---------------- HC-SR04 ----------------
int readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long dur = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (dur == 0) return 999;
  int cm = (int)(dur / 58UL);
  if (cm <= 0) cm = 999;
  return cm;
}

// ---------------- AVOID ----------------
void avoidObstacle() {
  // bu manevralar da kayda girsin ki geri dönerken geri sarabilsin
  drive(0,0,true); delay(40);

  drive(-backSpeed, -backSpeed, true); delay(220);
  drive(0,0,true); delay(60);

  // rastgele gibi: sağa pivot, sonra ileri
  drive(turnSpeed, -turnSpeed, true); delay(260);
  drive(0,0,true); delay(40);

  drive(baseSpeed, baseSpeed, true); delay(180);
}

// ---------------- SETUP/LOOP ----------------
void setup() {
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  motorStop();
}

void loop() {
  int L = readLineAvg(LINE_L);
  int R = readLineAvg(LINE_R);

  bool bL = isBlack(L);
  bool bR = isBlack(R);

  int d = readDistanceCm();

  // -------- RETURNING ----------
  if (state == RETURNING) {
    // engel çok yakınsa çarpmasın diye mini fren + biraz geri
    if (d <= OBSTACLE_CM) {
      motorStop(); delay(50);
      setMotorRaw(-120, -120); delay(120);
      motorStop(); delay(40);
    }

    if (retIndex < 0) {
      motorStop();
      state = DONE;
      return;
    }

    // aktif segmenti ters oynat
    Segment s = seg[retIndex];

    // segment başlangıcı ayarla
    if (retStartMs == 0) {
      retStartMs = millis();
      setMotorRaw(-s.l, -s.r);
    }

    if (millis() - retStartMs >= s.ms) {
      // bu segment bitti -> bir önceki segmente geç
      retIndex--;
      retStartMs = 0;
    }
    return;
  }

  if (state == DONE) {
    motorStop();
    return;
  }

  // -------- OBSTACLE PRIORITY ----------
  if (d <= OBSTACLE_CM) {
    avoidObstacle();
    return;
  }

  // -------- TARGET DETECTION ----------
  // İki sensör de siyah görüyorsa: hedefin üstündesin
  if (bL && bR) {
    finalizeLog();
    motorStop();
    delay(250); // hedefte “tamam” bekle

    // geri dönüş başlat
    retIndex = segCount - 1;
    retStartMs = 0;
    state = RETURNING;
    return;
  }

  // -------- APPROACH / SEARCH ----------
  // Bir sensör siyah görüyorsa siyaha doğru düzelt
  if (bL || bR) {
    state = APPROACH;
    int err = (R - L); // R>L ise sağ daha beyaz (sol daha siyah) gibi düşün
    int corr = constrain(err / 8, -steerGain, steerGain);

    int leftPWM  = baseSpeed - corr;
    int rightPWM = baseSpeed + corr;

    drive(leftPWM, rightPWM, true);
    return;
  }

  // Hiç siyah yoksa: SEARCH (yumuşak salınımlı gez)
  state = SEARCH;
  // salınım: her 900ms’de bir sağ/sol hafif bias
  unsigned long t = millis();
  int phase = (t / 900) % 2;
  int bias = (phase == 0) ? 35 : -35;

  drive(baseSpeed + bias, baseSpeed - bias, true);
}
