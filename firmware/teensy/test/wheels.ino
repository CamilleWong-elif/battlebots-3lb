#include <Servo.h>

// ============================================================
// Tank Drive - Left & Right Independently
// FlySky FS-i6: CH2 = Right Stick Up/Down, CH3 = Left Stick Up/Down
// ============================================================

// --- Pins ---
const int PIN_LEFT_OUT         = 6;   // Left drive ESC
const int PIN_RIGHT_OUT        = 5;   // Right drive ESC

const int PIN_RC_LEFT_DRIVE    = 4;   // CH3 - Left stick up/down
const int PIN_RC_RIGHT_DRIVE   = 3;   // CH2 - Right stick up/down

// --- ESC pulse limits ---
const int PWM_MIN = 1000;
const int PWM_MID = 1500;
const int PWM_MAX = 2000;

// --- Deadzone ---
const int DEADZONE = 50;

// --- ESC objects ---
Servo leftESC;
Servo rightESC;

// --- Helper: read RC channel safely ---
// Returns 0 if signal is lost or out of range
unsigned long readRC(int pin) {
  unsigned long val = pulseIn(pin, HIGH, 25000);
  if (val < 900 || val > 2100) return 0;
  return val;
}

// --- Helper: apply deadzone around center ---
int applyDeadzone(int val, int center, int deadzone) {
  if (abs(val - center) < deadzone) return center;
  return val;
}

// --- Helper: clamp ---
int clamp(int val, int minVal, int maxVal) {
  return max(minVal, min(maxVal, val));
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_RC_LEFT_DRIVE,  INPUT);
  pinMode(PIN_RC_RIGHT_DRIVE, INPUT);

  leftESC.attach(PIN_LEFT_OUT);
  rightESC.attach(PIN_RIGHT_OUT);

  Serial.println("=================================");
  Serial.println("     Tank Drive Control          ");
  Serial.println("=================================");

  // Arm ESCs at neutral (bidirectional ESCs arm at 1500)
  Serial.println("Arming drive ESCs at neutral...");
  leftESC.writeMicroseconds(PWM_MID);
  rightESC.writeMicroseconds(PWM_MID);
  delay(5000);

  Serial.println("ESCs armed. Ready to drive.");
  Serial.println();
}

void loop() {
  unsigned long leftRC  = readRC(PIN_RC_LEFT_DRIVE);
  unsigned long rightRC = readRC(PIN_RC_RIGHT_DRIVE);

  // --- FAILSAFE: stop motors if signal is lost ---
  if (leftRC == 0 || rightRC == 0) {
    leftESC.writeMicroseconds(PWM_MID);
    rightESC.writeMicroseconds(PWM_MID);
    Serial.println("FAILSAFE - Signal lost! Motors stopped.");
    delay(20);
    return;
  }

  // --- Apply deadzone ---
  int leftPWM  = applyDeadzone((int)leftRC,  PWM_MID, DEADZONE);
  int rightPWM = applyDeadzone((int)rightRC, PWM_MID, DEADZONE);

  // --- Clamp to safe range ---
  leftPWM  = clamp(leftPWM,  PWM_MIN, PWM_MAX);
  rightPWM = clamp(rightPWM, PWM_MIN, PWM_MAX);

  leftESC.writeMicroseconds(leftPWM);
  rightESC.writeMicroseconds(rightPWM);

  // --- Debug output ---
  Serial.print("L-RC: "); Serial.print(leftRC);
  Serial.print(" us | L-PWM: "); Serial.print(leftPWM);
  Serial.print(" || R-RC: "); Serial.print(rightRC);
  Serial.print(" us | R-PWM: "); Serial.println(rightPWM);

  delay(20); // ~50 Hz
}
