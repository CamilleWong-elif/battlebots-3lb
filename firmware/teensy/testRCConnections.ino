#include <Arduino.h>

const int PIN_RC_INPUT = 7;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RC_INPUT, INPUT);

  delay(1000);
  Serial.println("RC test starting...");
  Serial.println("Move the stick/switch and watch the values.");
  Serial.println("Expected range is usually about 1000 to 2000 us.");
}

void loop() {
  unsigned long pulseWidth = pulseIn(PIN_RC_INPUT, HIGH, 25000);

  if (pulseWidth == 0) {
    Serial.println("No signal");
  } else {
    Serial.print("RC pulse: ");
    Serial.print(pulseWidth);
    Serial.println(" us");
  }

  delay(100);
}