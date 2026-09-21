#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 9
#define RST_PIN 8
#define TRIGGER_PIN 13 // Ultrasonic sensor trigger pin
#define ECHO_PIN 12     // Ultrasonic sensor echo pin
#define IR_PIN_1 0
#define IR_PIN_2 1
#define IR_PIN_3 4

#define SERVO_PIN 2

LiquidCrystal_I2C LCD(0x27, 20, 4);
Servo servo;
MFRC522 rfid(SS_PIN, RST_PIN);

int irPin1 = IR_PIN_1;
int irPin2 = IR_PIN_2;
int irPin3 = IR_PIN_3;

int totalSpaces = 3; // Total parking spaces available
int occupiedSpaces = 0; // Initially, no parking spaces are occupied

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
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  pinMode(irPin3, INPUT);
}

void loop() {
  // Ultrasonic sensor distance measurement
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration / 58; // Convert the time into distance in centimeters

  int pirValue = (distance < 5) ? LOW : HIGH; // Adjust the distance threshold as needed

  if (pirValue == LOW) {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Please show your");
    LCD.setCursor(0, 1);
    LCD.print("ID card");
  }

  delay(1000);

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String tagID = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      tagID += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println("Tag ID: " + tagID);

    if (tagID == "c3a0f5ac" && occupiedSpaces < totalSpaces) {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Access granted!");
      servo.write(-90);
      delay(7000);
      servo.write(90);
      occupiedSpaces++;
    } else if (tagID == "c3a0f5ac" && occupiedSpaces >= totalSpaces) {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Parking Full");
      delay(1000);
    } else {
      LCD.clear();
      LCD.setCursor(0, 0);
      LCD.print("Access denied.");
      LCD.setCursor(0, 1);
      LCD.print("Please leave.");
      delay(1000);
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

  // Check for car presence using IR sensors
  if (irValue1 == HIGH) {
    // Car detected in slot 1
    if (occupiedSpaces < totalSpaces) {
      occupiedSpaces++;
    }
  }
  if (irValue2 == HIGH) {
    // Car detected in slot 2
    if (occupiedSpaces < totalSpaces) {
      occupiedSpaces++;
    }
  }
  if (irValue3 == HIGH) {
    // Car detected in slot 3
    if (occupiedSpaces < totalSpaces) {
      occupiedSpaces++;
    }
  }

  // Check for car departure using IR sensors
  if (irValue1 == LOW) {
    // Car left from slot 1
    if (occupiedSpaces > 0) {
      occupiedSpaces--;
    }
  }
  if (irValue2 == LOW) {
    // Car left from slot 2
    if (occupiedSpaces > 0) {
      occupiedSpaces--;
    }
  }
  if (irValue3 == LOW) {
    // Car left from slot 3
    if (occupiedSpaces > 0) {
      occupiedSpaces--;
    }
  }

  // Ensure clearSpaces stays within [0, totalSpaces]
  occupiedSpaces = constrain(occupiedSpaces, 0, totalSpaces);
  int clearSpaces = totalSpaces - occupiedSpaces;

  LCD.setCursor(0, 1);
  LCD.print("OS: ");
  LCD.print(occupiedSpaces);
  LCD.setCursor(7, 1);
  LCD.print("CS: ");
  LCD.print(clearSpaces);
  delay(500);
}
