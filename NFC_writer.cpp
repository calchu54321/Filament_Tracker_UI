#include "NFC_writer.h"
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <NdefMessage.h>
#include <NdefRecord.h>
#include "rotary_encoder.h"
#include "menu.h"


// PN532_I2C pn532_i2c(Wire);
// NfcAdapter nfc(pn532_i2c);
PN532_I2C pn532_i2c(Wire);
PN532 pn532(pn532_i2c);
NfcAdapter nfc(pn532_i2c); 


void initNFC() {
  pn532.begin(); 
  nfc.begin();
  Serial.println("NFC Initialized");
}

bool writeFilamentData(const String &materialType, const String &color, 
                       const String &spoolWeight, const String &tagName, 
                       float spoolWeightValue, float filamentUsed, float filamentLeft) {
  if (nfc.tagPresent()) {
    String json = "{\"material\":\"" + materialType +
                  "\",\"color\":\"" + color +
                  "\",\"spool\":\"" + spoolWeight +
                  "\",\"tag\":\"" + tagName +
                  "\",\"weight\":" + String(spoolWeightValue, 2) +
                  ",\"used\":" + String(filamentUsed, 2) +
                  ",\"left\":" + String(filamentLeft, 2) + "}";

    NdefMessage message = NdefMessage();
    message.addTextRecord(json);

    bool success = nfc.write(message);

    if (success) {
      Serial.println("Successfully wrote to NFC tag:");
      Serial.println(json);
    } else {
      Serial.println("Failed to write to NFC tag!");
    }

    return success;
  } else {
    Serial.println("No NFC tag detected!");
    return false;
  }
}

bool isTagPresentCustom() {
  uint8_t uid[7];      // Enough for most tags
  uint8_t uidLength;

  bool success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);  // 100ms timeout
  return success;
}

bool isTagPresent() {
  return nfc.tagPresent();
}