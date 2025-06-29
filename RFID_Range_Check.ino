#include <SPI.h>
#include <MFRC522.h>

const int SS_PIN = 5;   // פין SDA (SS) של קורא ה-RFID
const int RST_PIN = 22; // פין RST של קורא ה-RFID

MFRC522 mfrc522(SS_PIN, RST_PIN); // יצירת אובייקט MFRC522

void setup() {
  Serial.begin(115200);
  Serial.println("RFID Reader Example");
  SPI.begin();        // אתחול ממשק SPI
  mfrc522.PCD_Init();   // אתחול קורא ה-RFID
  Serial.println("Scan PICC to see UID...");
}

void loop() {
  // חפש כרטיסים חדשים
  if (mfrc522.PICC_IsNewCardPresent()) {
    // בחר אחד מהכרטיסים
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("PICC Detected! UID: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      Serial.println("Tag is in range!");
      // כאן תוכל להוסיף קוד נוסף שמתבצע כשהתג בטווח
      mfrc522.PICC_HaltA(); // עצור את הקריאה הנוכחית של ה-PICC
    }
  }
  delay(1000); // עיכוב קל
}