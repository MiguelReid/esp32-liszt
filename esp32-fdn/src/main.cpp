#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 alive from PlatformIO");
}

void loop() {
  delay(1000);
  Serial.println("tick");
}
