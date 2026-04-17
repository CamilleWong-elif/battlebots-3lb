#include <Arduino.h>

const int CURRENT_PIN = A0;

const float ADC_REF = 3.3f;
const int ADC_MAX = 1023;
const float SENSITIVITY = 0.044f;

float zeroVoltage = 1.667f;

void setup() {
  Serial.begin(115200);
  analogReadResolution(10);

  delay(2000);

  long sum = 0;
  const int calSamples = 500;
  for (int i = 0; i < calSamples; i++) {
    sum += analogRead(CURRENT_PIN);
    delay(2);
  }

  float avgADC = sum / (float)calSamples;
  zeroVoltage = (avgADC / ADC_MAX) * ADC_REF;

  Serial.println("time_ms,current_A");
}

void loop() {
  long sum = 0;
  const int samples = 100;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(CURRENT_PIN);
    delay(1);
  }

  float avgADC = sum / (float)samples;
  float voltage = (avgADC / ADC_MAX) * ADC_REF;
  float currentA = (voltage - zeroVoltage) / SENSITIVITY;

  if (currentA < 0) currentA = 0;

  Serial.print(millis());
  Serial.print(",");
  Serial.println(currentA, 3);

  delay(100);
}