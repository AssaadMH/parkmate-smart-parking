#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 9
#define RST_PIN 8
#define PIR_PIN 3
#define IR_PIN_1 0
#define IR_PIN_2 1
#define IR_PIN_3 4
#define IR_PIN_4 5
#define SERVO_PIN 2

LiquidCrystal_I2C LCD(0x27, 20, 4);
Servo servo;
MFRC522 rfid(SS_PIN, RST_PIN);

int pirPin = PIR_PIN;
int irPin1 = IR_PIN_1;
int irPin2 = IR_PIN_2;
int irPin3 = IR_PIN_3;
int irPin4 = IR_PIN_4;

int clearSpaces = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  LCD.init();
  LCD.backlight();
  LCD.setCursor(0, 0);
  LCD.print("Welcome ParkMate");
  LCD.setCursor(0, 1);
  LCD.print("ASSAAD MAHMOUDI");

  servo.attach(SERVO_PIN);

  pinMode(pirPin, INPUT);
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  pinMode(irPin3, INPUT);
  pinMode(irPin4, INPUT);
}

void loop() {
  int pirValue = digitalRead(pirPin);

  if (pirValue == HIGH) {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Please show your");
    LCD.setCursor(0, 1);
    LCD.print("ID card");
    if (clearSpaces > 0) {
      clearSpaces--; // Decrement clearSpaces when motion is detected
    }
  }

  delay(500);

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String tagID ="";
    for (byte i = 0; i < rfid.uid.size; i++) {
      tagID += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println("Tag ID: " + tagID);

    if (tagID == "b3d8891f") {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Access granted!");
      servo.write(90);
      delay(500);
      servo.write(0);
    } else {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Access denied.");
      LCD.setCursor(0, 1);
      LCD.print("Please leave.");
      delay(5000);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Please scan RFID");
    LCD.setCursor(0, 1);
    LCD.print("tag to proceed");
  }

  int irValue1 = digitalRead(irPin1);
  int irValue2 = digitalRead(irPin2);
  int irValue3 = digitalRead(irPin3);
  int irValue4 = digitalRead(irPin4);

  if (irValue1 == HIGH) {
    clearSpaces++;
    delay(1000);
  }
  if (irValue2 == HIGH) {
    clearSpaces++;
    delay(1000);
  }
  if (irValue3 == HIGH) {
    clearSpaces++;
    delay(1000);
  }
  if (irValue4 == HIGH) {
    clearSpaces++;
    delay(1000);
  }

  // Ensure clearSpaces stays within [0, 4]
  clearSpaces = constrain(clearSpaces, 0, 4);

  LCD.setCursor(0, 0);
  LCD.print("Clear Spaces: ");
  LCD.print(clearSpaces);
}
