#include "NFC_writer.h"
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <NdefMessage.h>
#include <NdefRecord.h>
#include "rotary_encoder.h"
#include "menu.h"
#include <ArduinoJson.h> 

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
                       float spoolWeightValue, float filamentAdjustLeftTEMP, long pulses) {
  if (nfc.tagPresent()) {
    String json = "{\"material\":\"" + materialType +
                  "\",\"color\":\"" + color +
                  "\",\"spool\":\"" + spoolWeight +
                  "\",\"tag\":\"" + tagName +
                  "\",\"weight\":" + String(spoolWeightValue, 2) +
                  ",\"filamenttemp\":" + String(filamentAdjustLeftTEMP, 2) +
                  ",\"pulses\":" + String(pulses) + "}";

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


bool readFilamentData(String &materialType, String &color, 
                      String &spoolWeight, String &tagName, 
                      float &spoolWeightValue, float &filamentAdjustLeftTEMP, long &pulses) {
  if (nfc.tagPresent()) {
    Serial.println("Tag detected, attempting to read...");
    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage()) {
      NdefMessage message = tag.getNdefMessage();
      NdefRecord record = message.getRecord(0);  // Assuming single record with JSON

      if (record.getTnf() == TNF_WELL_KNOWN && record.getType()[0] == 'T') {
        int payloadLength = record.getPayloadLength();
        Serial.print("Payload length: ");
        Serial.println(payloadLength);

        uint8_t payload[payloadLength];
        record.getPayload(payload);

        uint8_t statusByte = payload[0];
        uint8_t languageLength = statusByte & 0x3F; // lower 6 bits: language length
        bool isUTF16 = statusByte & 0x80; // highest bit indicates encoding (0=UTF8, 1=UTF16)
        
        Serial.print("Language length: ");
        Serial.println(languageLength);
        Serial.print("Encoding UTF-16? ");
        Serial.println(isUTF16 ? "Yes" : "No");

        // Extract text bytes after language code
        String payloadText = "";
        if (isUTF16) {
          Serial.println("UTF-16 encoding not supported, skipping");
          return false;
        } else {
          // UTF-8 text, read remaining bytes as chars
          for (int i = languageLength + 1; i < payloadLength; i++) {
            payloadText += (char)payload[i];
          }
        }

        Serial.println("Extracted JSON payload:");
        Serial.println(payloadText);

        // Optional: trim payloadText to last '}'
        int lastBraceIndex = payloadText.lastIndexOf('}');
        if (lastBraceIndex != -1) {
          payloadText = payloadText.substring(0, lastBraceIndex + 1);
        }

        // Parse JSON
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payloadText);
        if (error) {
          Serial.print("JSON Parse Error: ");
          Serial.println(error.c_str());
          return false;
        }

        // Update variables
        materialType = doc["material"].as<String>();
        color = doc["color"].as<String>();
        spoolWeight = doc["spool"].as<String>();
        tagName = doc["tag"].as<String>();
        spoolWeightValue = doc["weight"].as<float>();
        filamentAdjustLeftTEMP = doc["filamenttemp"].as<float>();
        pulses = doc["pulses"].as<long>();

        Serial.println("Parsed NFC Data:");
        Serial.print("materialType: "); Serial.println(materialType);
        Serial.print("color: "); Serial.println(color);
        Serial.print("spoolWeight: "); Serial.println(spoolWeight);
        Serial.print("tagName: "); Serial.println(tagName);
        Serial.print("spoolWeightValue: "); Serial.println(spoolWeightValue);
        Serial.print("filamentAdjustLeftTEMP: "); Serial.println(filamentAdjustLeftTEMP);
        Serial.print("pulses: "); Serial.println(pulses);

        return true;
      } else {
        Serial.println("NDEF record is not a well-known text record!");
        return false;
      }
    } else {
      Serial.println("No NDEF message found!");
      return false;
    }
  } else {
    Serial.println("No NFC tag detected!");
    return false;
  }
}


// custom for timing out nfc tag faster
bool isTagPresentCustom() {
  uint8_t uid[7];      // Enough for most tags
  uint8_t uidLength;

  bool success = pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);  // 100ms timeout
  return success;
}
