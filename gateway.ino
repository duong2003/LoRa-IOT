#include <Wire.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include "firebaseConnectionInfo.h" // File containing Firebase connection information
#include <BluetoothSerial.h>

#define FIREBASE_PROJECT_ID ""
#define FIREBASE_DATABASE_URL "https://" FIREBASE_PROJECT_ID ".firebaseio.com"
#define FIREBASE_SECRET "" // Secret key to access Firebase data

#define LORA_SS 18 // Define the chip select pin for the LoRa module.
#define LORA_RST 14 // Define the reset pin for the LoRa module.
#define LORA_DI0 26 // Define the DIO0 pin for interrupt handling.

LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize I2C LCD
FirebaseData firebaseData; // Initialize FirebaseData
BluetoothSerial SerialBT; // Initialize BluetoothSerial

#define RSSI_FILTER_WINDOW_SIZE 10 // Define the size of the moving average filter window for RSSI
int rssiBuffer[RSSI_FILTER_WINDOW_SIZE]; // Buffer to store RSSI values
int bufferIndex = 0; // Index for circular buffer
float filteredRSSI = 0; // Variable to store filtered RSSI value

void setup() {
  Serial.begin(9600); // Initialize serial with baudrate 9600
  while (!Serial); // Wait for serial connection to be established

  lcd.begin(); // Initialize LCD
  lcd.backlight(); // Turn on LCD backlight
  lcd.clear(); // Clear LCD screen

  if (!LoRa.begin(433E6)) { // Initialize LoRa module at 433MHz frequency
    Serial.println("Starting LoRa failed!"); // Print error message if initialization fails
    while (1); // Halt the program
  }

  // Configure LoRa module pins
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) { // Wait for WiFi connection
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Firebase.begin(FIREBASE_DATABASE_URL, FIREBASE_SECRET); // Connect to Firebase

  SerialBT.begin("ESP32_BT"); // Start Bluetooth with device name "ESP32_BT"
}

void loop() {
  // Receive LoRa packets
  int packetSize = LoRa.parsePacket(); // Check if there's a LoRa packet available
  if (packetSize) { // If a packet is available
    while (LoRa.available()) { // Loop through all available bytes
      String sensorData = LoRa.readString(); // Read the data from LoRa
      handleData(sensorData); // Handle the received data
      delay(3000); // Wait for 3 seconds before processing the next packet
    }
  }

  // Process Bluetooth data
  if (SerialBT.available()) { // Check if there's data available on Bluetooth
    String sensorData = SerialBT.readString(); // Read data from Bluetooth
    handleData(sensorData); // Handle the received data
    delay(3000); // Wait for 3 seconds before processing the next data
  }
}

void handleData(String data) {
  // Display data on LCD
  lcd.clear(); // Clear LCD screen
  lcd.setCursor(0, 0); // Set cursor to the beginning of the first line
  lcd.print("Received: "); // Print label
  lcd.setCursor(0, 1); // Set cursor to the beginning of the second line
  lcd.print(data); // Print received data

  // Push data to Firebase
  if (Firebase.pushString(firebaseData, "/sensorData", data)) { // Push data to Firebase
    Serial.println("Data pushed to Firebase!"); // Print success message
  } else {
    Serial.println("Pushing data to Firebase failed!"); // Print failure message
    Serial.println("Reason: " + firebaseData.errorReason()); // Print error reason
  }

  // Send data via LoRa
  LoRa.beginPacket(); // Start a LoRa packet
  LoRa.print(data); // Print the data
  LoRa.endPacket(); // Finish the LoRa packet

  // Calculate moving average RSSI
  int packetRSSI = LoRa.packetRssi(); // Get the RSSI of the received packet
  rssiBuffer[bufferIndex] = packetRSSI; // Store RSSI value in buffer
  bufferIndex = (bufferIndex + 1) % RSSI_FILTER_WINDOW_SIZE; // Update buffer index for circular behavior

  // Compute moving average
  float sum = 0;
  for (int i = 0; i < RSSI_FILTER_WINDOW_SIZE; i++) {
    sum += rssiBuffer[i];
  }
  filteredRSSI = sum / RSSI_FILTER_WINDOW_SIZE;
  Serial.print("Filtered RSSI: ");
  Serial.println(filteredRSSI);
}
