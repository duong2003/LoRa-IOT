#include <Wire.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4 // Chân kết nối với DS18B20
#define LORA_SS 18 // Define the chip select pin for the LoRa module.
#define LORA_RST 14 // Define the reset pin for the LoRa module.
#define LORA_DI0 26 // Define the DIO0 pin for interrupt handling.
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600); // Khởi tạo giao tiếp serial baudrate 9600 
  while (!Serial); // Đợi cho đến khi serial đã sẵn sàng

  if (!LoRa.begin(433E6)) { // Khởi tạo module LoRa ở tần số 433MHz
    Serial.println("Starting LoRa failed!"); // In ra serial thông báo lỗi nếu khởi tạo LoRa thất bại
    while (1); // Dừng chương trình
  }

  sensors.begin(); // Khởi tạo cảm biến DS18B20
}

void loop() {
  sensors.requestTemperatures(); // Gửi yêu cầu để cảm biến nhiệt độ đo nhiệt độ
  float temperature = sensors.getTempCByIndex(0); // Lấy giá trị nhiệt độ từ cảm biến và lưu vào biến temperature

  String data = "Temperature: " + String(temperature) + " C"; // Tạo một chuỗi dữ liệu chứa thông tin về nhiệt độ
  LoRa.beginPacket(); // Bắt đầu gửi gói tin LoRa
  LoRa.print(data); // Gửi dữ liệu thông qua LoRa
  LoRa.endPacket(); // Kết thúc gửi gói tin LoRa

  Serial.println("Sent: " + data); // In ra serial thông tin về dữ liệu đã gửi
  delay(5000); // Dừng chương trình trong 5 giây trước khi lặp lại quá trình gửi dữ liệu
}
