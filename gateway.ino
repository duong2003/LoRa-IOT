#include <Wire.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include "firebaseConnectionInfo.h" // File chứa thông tin kết nối Firebase
#include <BluetoothSerial.h>

#define FIREBASE_PROJECT_ID ""
#define FIREBASE_DATABASE_URL "https://" FIREBASE_PROJECT_ID ".firebaseio.com"
#define FIREBASE_SECRET "" // Secret key để truy cập dữ liệu Firebase

LiquidCrystal_I2C lcd(0x27, 16, 2); // Khởi tạo màn hình LCD I2C
FirebaseData firebaseData; // Khởi tạo FirebaseData
BluetoothSerial SerialBT; // Khởi tạo BluetoothSerial

void setup() {
  Serial.begin(9600); // Khởi tạo serial baudrate 9600
  while (!Serial); // Đợi đã sẵn sàng

  lcd.begin(); // Khởi tạo LCD
  lcd.backlight(); // Bật đèn nền LCD
  lcd.clear(); // Xóa LCD

  if (!LoRa.begin(433E6)) { // Khởi tạo module LoRa ở tần số 433MHz
    Serial.println("Starting LoRa failed!"); // In ra serial thông báo lỗi nếu khởi tạo LoRa thất bại
    while (1); // Dừng chương trình
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Kết nối WiFi
  while (WiFi.status() != WL_CONNECTED) { // Đợi khi kết nối WiFi thành công
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Firebase.begin(FIREBASE_DATABASE_URL, FIREBASE_SECRET); // Kết nối đến Firebase

  SerialBT.begin("ESP32_BT"); // Khởi động Bluetooth với tên thiết bị là "ESP32_BT"
}

void loop() {
  // Xử lý dữ liệu từ LoRa
  int packetSize = LoRa.parsePacket(); // Kiểm tra xem có gói tin nào từ LoRa không
  if (packetSize) { // Nếu có gói tin
    while (LoRa.available()) { // Vòng lặp để đọc toàn bộ dữ liệu từ LoRa
      String sensorData = LoRa.readString(); // Đọc dữ liệu từ LoRa
      handleData(sensorData); // Xử lý dữ liệu
      delay(3000); // Đợi 3 giây trước khi xử lý dữ liệu tiếp theo
    }
  }

  // Xử lý dữ liệu từ Bluetooth
  if (SerialBT.available()) { // Kiểm tra xem có dữ liệu từ Bluetooth không
    String sensorData = SerialBT.readString(); // Đọc dữ liệu từ Bluetooth
    handleData(sensorData); // Xử lý dữ liệu
    delay(3000); // Đợi 3 giây trước khi xử lý dữ liệu tiếp theo
  }
}

void handleData(String data) {
  // Hiển thị dữ liệu trên LCD
  lcd.clear(); // Xóa màn hình LCD
  lcd.setCursor(0, 0); // Đặt con trỏ ở vị trí (0, 0)
  lcd.print("Received: "); // In chuỗi "Received: "
  lcd.setCursor(0, 1); // Đặt con trỏ ở vị trí (0, 1)
  lcd.print(data); // In dữ liệu nhận được lên màn hình LCD

  // Gửi dữ liệu lên Firebase
  if (Firebase.pushString(firebaseData, "/sensorData", data)) { // Gửi dữ liệu lên Firebase
    Serial.println("Data pushed to Firebase!"); // In ra serial thông báo gửi dữ liệu thành công
  } else {
    Serial.println("Pushing data to Firebase failed!"); // In ra serial thông báo gửi dữ liệu thất bại
    Serial.println("Reason: " + firebaseData.errorReason()); // In ra serial lý do thất bại (nếu có)
  }

  // Gửi dữ liệu qua LoRa (nếu cần)
  LoRa.beginPacket(); // Bắt đầu gửi gói tin LoRa
  LoRa.print(data); // Gửi dữ liệu qua LoRa
  LoRa.endPacket(); // Kết thúc gửi gói tin LoRa
}
