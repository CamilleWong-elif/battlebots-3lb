#include <Arduino.h>
#include <Servo.h>

// ============================================================
// Battlebot Weapon Control with RC Input (Teensy 4.0)
// ============================================================

// --- Pins ---
const int PIN_WEAPON_OUT      = 22;   // ESC signal
const int PIN_RC_WEAPON_INPUT = 7;    // RC receiver signal

// --- ESC Limits ---
const int PWM_MIN = 1000;
const int PWM_MAX = 2000;

// --- Threshold ---
const int WEAPON_ON_THRESHOLD = 1500;

// --- Objects ---
Servo weaponESC;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_RC_WEAPON_INPUT, INPUT);
  weaponESC.attach(PIN_WEAPON_OUT, PWM_MIN, PWM_MAX);

  Serial.println("Booting...");

  // Arm ESC safely
  Serial.println("Arming ESC...");
  weaponESC.writeMicroseconds(PWM_MIN);
  delay(8000);

  Serial.println("ESC armed. Waiting for RC input...");
}

void loop() {

  // --------------------------------------------------
  // 1. READ RC SIGNAL
  // --------------------------------------------------
  int rcChannelValue = pulseIn(PIN_RC_WEAPON_INPUT, HIGH, 25000);

  // --------------------------------------------------
  // 2. FAILSAFE (signal lost)
  // --------------------------------------------------
  if (rcChannelValue == 0) {
    // No valid signal → STOP
    weaponESC.writeMicroseconds(PWM_MIN);
    Serial.println("FAILSAFE: No signal");
    delay(20);
    return;
  }

  // --------------------------------------------------
  // 3. DECISION LOGIC
  // --------------------------------------------------
  int weaponCommand;

  if (rcChannelValue > WEAPON_ON_THRESHOLD) {
    weaponCommand = PWM_MAX;  // ON
    Serial.println("Weapon ON");
  } else {
    weaponCommand = PWM_MIN;  // OFF
    Serial.println("Weapon OFF");
  }

  // --------------------------------------------------
  // 4. OUTPUT TO ESC
  // --------------------------------------------------
  weaponESC.writeMicroseconds(weaponCommand);

  // Debug print (optional)
  Serial.print("RC: ");
  Serial.print(rcChannelValue);
  Serial.print(" | PWM: ");
  Serial.println(weaponCommand);

  delay(20); // ~50Hz update rate
}