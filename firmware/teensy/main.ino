// ============================================================
// Battlebot Weapon Spin-Up
// Teensy 4.0 + Vortex 80A ESC + 2836 Brushless Motor
// ============================================================
// Wiring:
//   Weapon ESC signal wire -> PIN_WEAPON_OUT (Pin 3)
//   All grounds tied together
// ============================================================
#include <Arduino.h>

// --- Pin Definitions ---
const int PIN_THROTTLE_IN = 7;   // Unused (kept for reference)
const int PIN_KILL_IN     = 6;   // Unused (kept for reference)
const int PIN_WEAPON_OUT  = 3;   // Output to weapon ESC
const int PIN_DRIVE_OUT   = 4;   // Unused (kept for reference)

// --- PWM Pulse Limits ---
const int PWM_MIN         = 1000; // microseconds (off / ESC arm signal)
const int PWM_MAX         = 2000; // microseconds (full speed)

// --- Ramp Parameters ---
const int   RAMP_STEP_US  = 100;   // microseconds per step
const int   RAMP_DELAY_MS = 20;   // milliseconds between steps

// Helper: send a single PWM pulse on PIN_WEAPON_OUT
void sendPulse(int pulseWidth_us) {
  digitalWrite(PIN_WEAPON_OUT, HIGH);
  delayMicroseconds(pulseWidth_us);
  digitalWrite(PIN_WEAPON_OUT, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_WEAPON_OUT, OUTPUT);

  delay(3000);
  Serial.println("Boot delay complete, starting ESC sequence...");

  // ----------------------------------------------------------
  // Phase 1: ESC Arming Sequence
  // Send PWM_MIN (1000us) pulses for ~2 seconds so the ESC
  // initialises and recognises the throttle-low position.
  // At 20ms per cycle that's ~100 pulses.
  // ----------------------------------------------------------
  Serial.println("=== ESC Arming: sending 1000us for 2s ===");
  for (int i = 0; i < 100; i++) {
    sendPulse(PWM_MIN);
    delay(RAMP_DELAY_MS); // 100 x 20ms = 2000ms
  }
  Serial.println("Arming complete.");

  // ----------------------------------------------------------
  // Phase 2: Smooth Ramp 1000us -> 2000us
  // 10us steps every 20ms = 100 steps x 20ms = 2000ms
  // ----------------------------------------------------------
  Serial.println("=== Ramping weapon ESC to full speed ===");
  for (int pw = PWM_MIN; pw <= PWM_MAX; pw += RAMP_STEP_US) {
    sendPulse(pw);
    Serial.print("Weapon PWM: ");
    Serial.print(pw);
    Serial.println("us");
    delay(RAMP_DELAY_MS);
  }
  Serial.println("=== Full speed reached: holding at 2000us ===");
}

void loop() {
  // Hold at full speed indefinitely
  sendPulse(PWM_MAX);

  // Debug heartbeat — prints every ~100 pulses to avoid flooding Serial
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 2000) {
    Serial.println("Weapon PWM: 2000us (holding)");
    lastPrint = millis();
  }

  delay(RAMP_DELAY_MS); // maintain ~50Hz pulse rate
}
