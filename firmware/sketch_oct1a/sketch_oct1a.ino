#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 9
#define RST_PIN 8

LiquidCrystal_I2C LCD(0x27, 20, 4);
Servo servo;
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600); // Initialize Serial communication
  SPI.begin();
  rfid.PCD_Init();
  LCD.init();
  LCD.backlight();
  LCD.setCursor(3, 0);
  LCD.print("Welcome ParkMate");
  LCD.setCursor(2, 1);
  LCD.print("ASSAAD MAHMOUDI");

}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String tagID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      tagID += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println("Tag ID: " + tagID); // Print tag ID to Serial Monitor

    if (tagID == "b3d8891f") {
      Serial.println("Access granted!"); // Print access message to Serial Monitor
      servo.write(90);
      delay(500);
      servo.write(0);
    } else {
      Serial.println("Access denied. Please leave."); // Print denial message to Serial Monitor
      delay(500);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Please scan RFID");
    LCD.setCursor(0, 1);
    LCD.print("tag to proceed");
  }
}
