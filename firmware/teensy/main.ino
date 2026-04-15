// ============================================================
// Battlebot Weapon Kill Switch
// Teensy 4.0 + FlySky i6
// ============================================================
// Wiring:
//   FlySky receiver CH3 (throttle) -> PIN_THROTTLE_IN
//   FlySky receiver CH5 (kill switch) -> PIN_KILL_IN
//   Weapon ESC signal wire          -> PIN_WEAPON_OUT
//   Drive ESC signal wire           -> PIN_DRIVE_OUT  (passthrough)
//   All grounds tied together
// ============================================================

#include <Arduino.h>

// --- Pin Definitions ---
const int PIN_THROTTLE_IN = 7;   // Drive throttle from receiver (CH3)
const int PIN_KILL_IN     = 6;   // Kill switch channel from receiver (CH5 or CH6)
const int PIN_WEAPON_OUT  = 3;   // Output to weapon ESC
const int PIN_DRIVE_OUT   = 4;   // Output to drive ESC (passthrough)

// --- PWM Pulse Limits ---
const int PWM_MIN         = 1000;  // microseconds (full reverse / off)
const int PWM_MID         = 1500;  // microseconds (neutral)
const int PWM_MAX         = 2000;  // microseconds (full forward)

// --- Kill Switch Threshold ---
// FlySky 3-pos switch: ~1000 (down), ~1500 (mid), ~2000 (up)
// We treat anything above 1700us as "ARMED"
const int KILL_THRESHOLD  = 1700;

// --- Safety: pulse timeout (if no signal, kill weapon) ---
const unsigned long SIGNAL_TIMEOUT_MS = 250;

unsigned long lastThrottleTime = 0;
unsigned long lastKillTime     = 0;

// Read a PWM pulse width in microseconds from a pin
// Returns 0 on timeout
unsigned long readPWM(int pin) {
  unsigned long duration = pulseIn(pin, HIGH, 25000); // 25ms timeout
  return duration;
}

void setup() {
  pinMode(PIN_THROTTLE_IN, INPUT);
  pinMode(PIN_KILL_IN,     INPUT);
  pinMode(PIN_WEAPON_OUT,  OUTPUT);
  pinMode(PIN_DRIVE_OUT,   OUTPUT);

  // Send safe/disarmed signal to weapon ESC on startup
  // ESCs need to see a LOW (1000us) pulse to arm safely
  for (int i = 0; i < 100; i++) {
    digitalWrite(PIN_WEAPON_OUT, HIGH);
    delayMicroseconds(PWM_MIN);
    digitalWrite(PIN_WEAPON_OUT, LOW);
    delay(20);
  }

  Serial.begin(115200);
  Serial.println("Battlebot controller initialized. Weapon DISARMED.");
}

void loop() {
  unsigned long now = millis();

  // --- Read kill switch channel ---
  unsigned long killPulse = readPWM(PIN_KILL_IN);
  if (killPulse > 0) lastKillTime = now;

  // --- Read throttle / drive channel ---
  unsigned long throttlePulse = readPWM(PIN_THROTTLE_IN);
  if (throttlePulse > 0) lastThrottleTime = now;

  // --- Signal loss safety: kill weapon if receiver goes silent ---
  bool signalLost = (now - lastKillTime > SIGNAL_TIMEOUT_MS) ||
                    (now - lastThrottleTime > SIGNAL_TIMEOUT_MS);

  // --- Determine armed state ---
  // Switch UP (>1700us) = ARMED, anything else = DISARMED
  bool weaponArmed = (!signalLost) && (killPulse >= KILL_THRESHOLD);

  // --- Output to weapon ESC ---
  unsigned long weaponPulse = weaponArmed ? throttlePulse : PWM_MIN;

  // Clamp to safe range
  weaponPulse = constrain(weaponPulse, PWM_MIN, PWM_MAX);
  unsigned long drivePulse = constrain(throttlePulse, PWM_MIN, PWM_MAX);

  // --- Send weapon PWM ---
  digitalWrite(PIN_WEAPON_OUT, HIGH);
  delayMicroseconds(weaponPulse);
  digitalWrite(PIN_WEAPON_OUT, LOW);

  // --- Send drive PWM passthrough ---
  digitalWrite(PIN_DRIVE_OUT, HIGH);
  delayMicroseconds(drivePulse);
  digitalWrite(PIN_DRIVE_OUT, LOW);

  // --- Debug output ---
  Serial.print("Kill: "); Serial.print(killPulse);
  Serial.print("us | Armed: "); Serial.print(weaponArmed ? "YES" : "NO");
  Serial.print(" | Weapon PWM: "); Serial.print(weaponPulse);
  Serial.print("us | Drive PWM: "); Serial.println(drivePulse);

  delay(5); // ~50Hz loop
}