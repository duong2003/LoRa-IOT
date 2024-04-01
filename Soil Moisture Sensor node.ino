#include <Wire.h>
#include <LoRa.h>

#define SOIL_MOISTURE_PIN 5 // Chân kết nối với Soil Moisture Sensor

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(SOIL_MOISTURE_PIN, INPUT);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  
  String data = "Soil Moisture: " + String(soilMoisture);
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  Serial.println("Sent: " + data);
  delay(5000); // Gửi dữ liệu mỗi 5 giây
}
