#include <Wire.h>
#include <LoRa.h>

#define SOIL_MOISTURE_PIN 5 // Pin connected to Soil Moisture Sensor
#define LORA_CS_PIN      18  // LoRa chip select pin
#define LORA_RST_PIN     14  // LoRa reset pin
#define LORA_IRQ_PIN     26  // LoRa interrupt pin

void setup() {
  Serial.begin(9600); // Initialize serial communication
  while (!Serial); // Wait until serial port is ready

  pinMode(SOIL_MOISTURE_PIN, INPUT); // Set Soil Moisture Sensor pin as input

  if (!LoRa.begin(433E6)) { // Initialize LoRa module at 433MHz frequency
    Serial.println("Starting LoRa failed!"); // Print error message if LoRa initialization fails
    while (1); // Halt the program
  }

  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_IRQ_PIN); // Set LoRa pins
}

void loop() {
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN); // Read soil moisture value from sensor
  
  String data = "Soil Moisture: " + String(soilMoisture); // Create data string containing soil moisture value
  LoRa.beginPacket(); // Start sending LoRa packet
  LoRa.print(data); // Send data via LoRa
  LoRa.endPacket(); // End LoRa packet transmission

  Serial.println("Sent: " + data); // Print sent data to serial monitor
  delay(5000); // Wait for 5 seconds before sending next data
}
