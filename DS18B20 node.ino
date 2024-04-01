#include <Wire.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4 // Chân kết nối với DS18B20

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  String data = "Temperature: " + String(temperature) + " C";
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  Serial.println("Sent: " + data);
  delay(5000); // Gửi dữ liệu mỗi 5 giây
}
