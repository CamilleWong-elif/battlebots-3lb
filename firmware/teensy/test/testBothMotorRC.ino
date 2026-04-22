#include <Servo.h>

// ============================================================
// Weapon Control + RC Signal Print
// Arduino Uno / Elegoo Uno R3
// ============================================================

// --- Pins ---
const int PIN_WEAPON_OUT      = 9;   // ESC signal output
const int PIN_RC_WEAPON_INPUT = 7;   // RC receiver signal input

// --- ESC pulse limits ---
const int PWM_MIN = 1000;   // weapon OFF
const int PWM_MAX = 2000;   // weapon ON

// --- Threshold ---
const int WEAPON_ON_THRESHOLD = 1500;

// --- ESC object ---
Servo weaponESC;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RC_WEAPON_INPUT, INPUT);
  weaponESC.attach(PIN_WEAPON_OUT);

  Serial.println("=================================");
  Serial.println("Weapon Control + RC Monitor Start");
  Serial.println("=================================");

  // Arm ESC safely
  Serial.println("Arming ESC at minimum throttle...");
  weaponESC.writeMicroseconds(PWM_MIN);
  delay(5000);

  Serial.println("ESC armed.");
  Serial.println("Monitoring RC input...");
  Serial.println();
}

void loop() {
  // Read RC signal
  unsigned long rcValue = pulseIn(PIN_RC_WEAPON_INPUT, HIGH, 25000);

  // Print raw RC signal
  Serial.print("RC Pulse: ");
  Serial.print(rcValue);
  Serial.print(" us");

  // Failsafe
  if (rcValue < 900 || rcValue > 2100) {
    weaponESC.writeMicroseconds(PWM_MIN);
    Serial.print(" | Status: FAILSAFE");
    Serial.print(" | PWM Out: ");
    Serial.println(PWM_MIN);
    delay(20);
    return;
  }

  // Weapon ON/OFF control
  if (rcValue > WEAPON_ON_THRESHOLD) {
    weaponESC.writeMicroseconds(PWM_MAX);
    Serial.print(" | Status: WEAPON ON");
    Serial.print(" | PWM Out: ");
    Serial.println(PWM_MAX);
  } else {
    weaponESC.writeMicroseconds(PWM_MIN);
    Serial.print(" | Status: WEAPON OFF");
    Serial.print(" | PWM Out: ");
    Serial.println(PWM_MIN);
  }

  delay(20); // ~50 Hz
}