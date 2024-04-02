#include <Wire.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4 // Pin connected to DS18B20
#define LORA_SS 18 // Define the chip select pin for the LoRa module.
#define LORA_RST 14 // Define the reset pin for the LoRa module.
#define LORA_DI0 26 // Define the DIO0 pin for interrupt handling.

OneWire oneWire(ONE_WIRE_BUS); // Initialize OneWire bus with the defined pin.

DallasTemperature sensors(&oneWire); // Pass the oneWire reference to DallasTemperature library.

void setup() {
  Serial.begin(9600); // Initialize serial communication with baudrate 9600
  while (!Serial); // Wait until serial is ready

  if (!LoRa.begin(433E6)) { // Initialize LoRa module at 433MHz frequency
    Serial.println("Starting LoRa failed!"); // Print error message to serial if LoRa initialization fails
    while (1); // Halt the program
  }

  sensors.begin(); // Initialize DS18B20 sensor
}

void loop() {
  sensors.requestTemperatures(); // Send a request to the temperature sensor to measure temperature
  float temperature = sensors.getTempCByIndex(0); // Get temperature value from the sensor and store it in 'temperature' variable

  String data = "Temperature: " + String(temperature) + " C"; // Create a data string containing temperature information
  LoRa.beginPacket(); // Start sending LoRa packet
  LoRa.print(data); // Send data via LoRa
  LoRa.endPacket(); // End LoRa packet transmission

  Serial.println("Sent: " + data); // Print information about the sent data to serial
  delay(5000); // Delay program for 5 seconds before repeating the data transmission process
}
