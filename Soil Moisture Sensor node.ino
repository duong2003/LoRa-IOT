#include <Wire.h>
#include <LoRa.h>

#define SOIL_MOISTURE_PIN 5 // Chân kết nối với Soil Moisture Sensor
#define LORA_SS 18 // Define the chip select pin for the LoRa module.
#define LORA_RST 14 // Define the reset pin for the LoRa module.
#define LORA_DI0 26 // Define the DIO0 pin for interrupt handling.

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
