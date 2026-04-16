#include <Arduino.h>
#include <Servo.h>

Servo esc;

const int ESC_PIN = 22;

void setup() {
  Serial.begin(115200);

  esc.attach(ESC_PIN);

  Serial.println("Starting ESC test...");

  // Send minimum throttle BEFORE ESC power
  esc.writeMicroseconds(1000);
  Serial.println("Sending 1000us (arm)");

  delay(8000);  // give ESC time to arm

  // Slowly increase throttle
  Serial.println("Trying 1100us...");
  esc.writeMicroseconds(1100);
  delay(4000);

  Serial.println("Trying 1200us...");
  esc.writeMicroseconds(1200);
  delay(4000);

  // Stop motor
  Serial.println("Stopping...");
  esc.writeMicroseconds(1000);
}

void loop() {
  // Keep sending valid signal
  esc.writeMicroseconds(1000);
  delay(20);
}