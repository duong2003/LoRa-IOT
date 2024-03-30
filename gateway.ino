#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LoRa.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("LoRa Gateway");
}

void loop() {
  if (LoRa.parsePacket()) {
    float temperature = LoRa.parseFloat();
    Serial.print("Received temperature: ");
    Serial.println(temperature);

    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print("C");
  }
}
