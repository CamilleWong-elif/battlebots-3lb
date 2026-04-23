#include <Arduino.h>
#include <Servo.h>

Servo esc;

const int ESC_PIN = 9;

int currentPWM = 1000; // start stopped

void setup() {
  Serial.begin(115200);

  esc.attach(ESC_PIN, 1000, 2000);

  Serial.println("Starting ESC test...");

  // Arm ESC
  esc.writeMicroseconds(1000);
  Serial.println("Arming at 1000us...");
  delay(8000);

  Serial.println("Ready!");
  Serial.println("Type 1 = ON, 0 = OFF");
}

void loop() {
  // Check for keyboard input
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == '1') {
      currentPWM = 1300;  // spin
      Serial.println("Motor ON");
    }
    else if (cmd == '0') {
      currentPWM = 1000;  // stop
      Serial.println("Motor OFF");
    }
  }

  // Keep sending signal
  esc.writeMicroseconds(currentPWM);
  delay(20);
}