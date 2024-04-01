#include <Wire.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include "firebaseConnectionInfo.h" // File chứa thông tin kết nối Firebase
#include <BluetoothSerial.h>

#define FIREBASE_PROJECT_ID ""
#define FIREBASE_DATABASE_URL "https://" FIREBASE_PROJECT_ID ".firebaseio.com"
#define FIREBASE_SECRET "" // Secret key để truy cập dữ liệu Firebase

LiquidCrystal_I2C lcd(0x27, 16, 2);
FirebaseData firebaseData;
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  lcd.begin();
  lcd.backlight();
  lcd.clear();

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Firebase.begin(FIREBASE_DATABASE_URL, FIREBASE_SECRET);

  SerialBT.begin("ESP32_BT"); // Khởi động Bluetooth với tên thiết bị là "ESP32_BT"
}

void loop() {
  // Xử lý dữ liệu từ LoRa
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      String sensorData = LoRa.readString();
      handleData(sensorData);
      delay(3000);
    }
  }

  // Xử lý dữ liệu từ Bluetooth
  if (SerialBT.available()) {
    String sensorData = SerialBT.readString();
    handleData(sensorData);
    delay(3000);
  }
}

void handleData(String data) {
  // Hiển thị dữ liệu trên LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Received: ");
  lcd.setCursor(0, 1);
  lcd.print(data);

  // Gửi dữ liệu lên Firebase
  if (Firebase.pushString(firebaseData, "/sensorData", data)) {
    Serial.println("Data pushed to Firebase!");
  } else {
    Serial.println("Pushing data to Firebase failed!");
    Serial.println("Reason: " + firebaseData.errorReason());
  }

  // Gửi dữ liệu qua LoRa (nếu cần)
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();
}
