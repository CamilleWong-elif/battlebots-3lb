#include <Servo.h>

// ============================================================
// Drive Test Without Kill Switch Lockout
// CH3 -> Left wheel
// CH2 -> Right wheel
// ============================================================

const int PIN_CH2 = 2;
const int PIN_CH3 = 3;

const int PIN_ESC_LEFT  = 5;
const int PIN_ESC_RIGHT = 6;

Servo escLeft;
Servo escRight;

const int PWM_MIN = 1000;
const int PWM_MAX = 2000;

volatile unsigned long ch2RiseTime = 0;
volatile unsigned long ch3RiseTime = 0;
volatile unsigned int ch2Pulse = 0;
volatile unsigned int ch3Pulse = 0;

void isrCH2() {
  if (digitalRead(PIN_CH2) == HIGH) {
    ch2RiseTime = micros();
  } else {
    ch2Pulse = (unsigned int)(micros() - ch2RiseTime);
  }
}

void isrCH3() {
  if (digitalRead(PIN_CH3) == HIGH) {
    ch3RiseTime = micros();
  } else {
    ch3Pulse = (unsigned int)(micros() - ch3RiseTime);
  }
}

bool isValidPulse(unsigned long pulse) {
  return pulse >= 900 && pulse <= 2100;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_CH2, INPUT);
  pinMode(PIN_CH3, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_CH2), isrCH2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CH3), isrCH3, CHANGE);

  escLeft.attach(PIN_ESC_LEFT);
  escRight.attach(PIN_ESC_RIGHT);

  escLeft.writeMicroseconds(PWM_MIN);
  escRight.writeMicroseconds(PWM_MIN);
  delay(5000);

  Serial.println("Drive test started.");
}

void loop() {
  noInterrupts();
  unsigned int ch2 = ch2Pulse;
  unsigned int ch3 = ch3Pulse;
  interrupts();

  int outLeft = PWM_MIN;
  int outRight = PWM_MIN;

  if (isValidPulse(ch3)) {
    outLeft = ch3;
  }

  if (isValidPulse(ch2)) {
    outRight = ch2;
  }

  outLeft = constrain(outLeft, PWM_MIN, PWM_MAX);
  outRight = constrain(outRight, PWM_MIN, PWM_MAX);

  escLeft.writeMicroseconds(outLeft);
  escRight.writeMicroseconds(outRight);

  Serial.print("CH2: ");
  Serial.print(ch2);
  Serial.print(" | CH3: ");
  Serial.print(ch3);
  Serial.print(" || Left: ");
  Serial.print(outLeft);
  Serial.print(" | Right: ");
  Serial.println(outRight);

  delay(20);
}