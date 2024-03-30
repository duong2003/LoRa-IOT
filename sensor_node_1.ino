#include <Wire.h>
#include <LoRa.h>

#define LM35_PIN A0

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  float temperature = readLM35();

  Serial.print("Sending temperature from Sensor Node 1: ");
  Serial.println(temperature);

  LoRa.beginPacket();
  LoRa.print(temperature);
  LoRa.endPacket();

  delay(5000); // Send temperature every 5 seconds
}

float readLM35() {
  int val = analogRead(LM35_PIN);
  float mv = (val / 1024.0) * 5000; // Convert to millivolts
  return (mv / 10); // LM35 outputs 10mV per degree Celsius
}
