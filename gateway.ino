#include <Wire.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include "firebaseConnectionInfo.h" // File chứa thông tin kết nối Firebase

#define FIREBASE_PROJECT_ID ""
#define FIREBASE_DATABASE_URL "https://" FIREBASE_PROJECT_ID ".firebaseio.com"
#define FIREBASE_SECRET "" // Secret key để truy cập dữ liệu Firebase

LiquidCrystal_I2C lcd(0x27, 16, 2);
FirebaseData firebaseData;

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
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      String sensorData = LoRa.readString();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Received: ");
      lcd.setCursor(0, 1);
      lcd.print(sensorData);

      if (Firebase.pushString(firebaseData, "/sensorData", sensorData)) {
        Serial.println("Data pushed to Firebase!");
      } else {
        Serial.println("Pushing data to Firebase failed!");
        Serial.println("Reason: " + firebaseData.errorReason());
      }

      delay(3000);
    }
  }
}
