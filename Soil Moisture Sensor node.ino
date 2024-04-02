#include <Wire.h>
#include <LoRa.h>

#define SOIL_MOISTURE_PIN 5 // Pin connected to Soil Moisture Sensor
#define LORA_CS_PIN      18
#define LORA_RST_PIN     14
#define LORA_IRQ_PIN     26

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(SOIL_MOISTURE_PIN, INPUT);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_IRQ_PIN);
}

void loop() {
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  
  String data = "Soil Moisture: " + String(soilMoisture);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  Serial.println("Sent: " + data);
  delay(5000); // Send data every 5 seconds
}
