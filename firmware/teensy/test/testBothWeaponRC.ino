#include <Servo.h>

const int PIN_WEAPON_OUT = 9;
const int PIN_RC_WEAPON_INPUT = 2;   // must be interrupt pin on Uno
const int PWM_MIN = 1000;
const int PWM_MAX = 2000;

Servo weaponESC;

volatile unsigned long riseTime = 0;
volatile unsigned int rcValue = 1500;
volatile bool newPulse = false;

void rcISR() {
  if (digitalRead(PIN_RC_WEAPON_INPUT) == HIGH) {
    riseTime = micros();
  } else {
    unsigned long width = micros() - riseTime;
    if (width >= 900 && width <= 2100) {
      rcValue = width;
      newPulse = true;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RC_WEAPON_INPUT, INPUT);
  weaponESC.attach(PIN_WEAPON_OUT);

  attachInterrupt(digitalPinToInterrupt(PIN_RC_WEAPON_INPUT), rcISR, CHANGE);

  Serial.println("Arming ESC...");
  weaponESC.writeMicroseconds(PWM_MIN);
  delay(5000);
  Serial.println("ESC armed.");
}

void loop() {
  static unsigned long lastGoodSignal = 0;
  static unsigned long lastPrint = 0;

  noInterrupts();
  unsigned int currentRC = rcValue;
  bool gotPulse = newPulse;
  newPulse = false;
  interrupts();

  if (gotPulse) {
    lastGoodSignal = millis();
    int pwmOut = constrain(currentRC, PWM_MIN, PWM_MAX);
    weaponESC.writeMicroseconds(pwmOut);

    if (millis() - lastPrint > 100) {
      Serial.print("RC: ");
      Serial.print(currentRC);
      Serial.print("  PWM Out: ");
      Serial.println(pwmOut);
      lastPrint = millis();
    }
  } else if (millis() - lastGoodSignal > 100) {
    weaponESC.writeMicroseconds(PWM_MIN);

    if (millis() - lastPrint > 200) {
      Serial.println("FAILSAFE");
      lastPrint = millis();
    }
  }
}