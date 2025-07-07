#include "NFC_reader.h"
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>


// Create PN532 and NfcAdapter instances (Wire must be started externally)
PN532_I2C pn532_i2c(Wire);
PN532 pn532(pn532_i2c);
NfcAdapter nfc(pn532_i2c);

TagData tagData;


// Definitions
TagData::TagData()
  : mat(""), col(""), spo(""), tag(""), wei(0.0), lef(0),
    matFilled(false), colFilled(false), spoFilled(false),
    tagFilled(false), weiFilled(false), lefFilled(false) {}

bool TagData::isComplete() {
  return matFilled && colFilled && spoFilled &&
         tagFilled && weiFilled && lefFilled;
}

void TagData::reset() {
  *this = TagData();
}

bool readAndParseNFC() {
  if (!nfc.tagPresent()) return false;

  NfcTag tag = nfc.read();

  String payload = getTextRecordPayload(tag);
  if (payload.length() == 0) return false;

  int startIndex = payload.indexOf('{');
  if (startIndex == -1) return false;

  String jsonPart = payload.substring(startIndex);

  parseAndUpdateTagData(jsonPart);

  return tagData.isComplete();
}

String getTextRecordPayload(NfcTag &tag) {
  if (!tag.hasNdefMessage()) return "";

  NdefMessage message = tag.getNdefMessage();
  for (int i = 0; i < message.getRecordCount(); i++) {
    NdefRecord record = message.getRecord(i);
    if (record.getType() == "T") {  // Text record
      uint8_t payloadLength = record.getPayloadLength();
      if (payloadLength > 0) {
        uint8_t payload[payloadLength];
        record.getPayload(payload);

        uint8_t status = payload[0];
        uint8_t langLength = status & 0x3F;

        String textPayload = "";
        for (int j = 1 + langLength; j < payloadLength; j++) {
          textPayload += (char)payload[j];
        }
        return textPayload;
      }
    }
  }
  return "";
}

void parseAndUpdateTagData(String json) {
  const char* keys[] = {"mat", "col", "spo", "tag", "wei", "lef"}; // material, color, spoolWeight, tagName, Weight, Filamentleft
  const int numKeys = 6;

  for (int i = 0; i < numKeys; i++) {
    if (fieldFilled(i)) continue;

    String key = keys[i];
    String searchPattern = "\"" + key + "\"";

    int keyIndex = json.indexOf(searchPattern);
    if (keyIndex == -1) continue;

    int colonIndex = json.indexOf(':', keyIndex + key.length());
    if (colonIndex == -1) continue;

    int valueStart = colonIndex + 1;
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t')) valueStart++;

    if (valueStart >= json.length()) continue;

    if (isStringField(i)) {
      if (json[valueStart] != '"') continue;

      int valueEnd = json.indexOf('"', valueStart + 1);
      if (valueEnd == -1) continue;

      String value = json.substring(valueStart + 1, valueEnd);
      if (value.length() == 0) continue;

      updateField(i, value);
    } else {
      int valueEnd = json.indexOf(',', valueStart);
      if (valueEnd == -1) valueEnd = json.indexOf('}', valueStart);
      if (valueEnd == -1) continue;

      String valueStr = json.substring(valueStart, valueEnd);
      valueStr.trim();
      if (valueStr.length() == 0) continue;

      if (!isNumber(valueStr)) continue;

      updateField(i, valueStr);
    }
  }
}

bool fieldFilled(int index) {
  switch (index) {
    case 0: return tagData.matFilled;
    case 1: return tagData.colFilled;
    case 2: return tagData.spoFilled;
    case 3: return tagData.tagFilled;
    case 4: return tagData.weiFilled;
    case 5: return tagData.lefFilled;
    default: return false;
  }
}

bool isStringField(int index) {
  return (index >= 0 && index <= 3);
}

void updateField(int index, String val) {
  switch (index) {
    case 0:
      tagData.mat = val;
      tagData.matFilled = true;
      Serial.print("Updated mat: "); Serial.println(val);
      break;
    case 1:
      tagData.col = val;
      tagData.colFilled = true;
      Serial.print("Updated col: "); Serial.println(val);
      break;
    case 2:
      tagData.spo = val;
      tagData.spoFilled = true;
      Serial.print("Updated spo: "); Serial.println(val);
      break;
    case 3:
      tagData.tag = val;
      tagData.tagFilled = true;
      Serial.print("Updated tag: "); Serial.println(val);
      break;
    case 4:
      tagData.wei = val.toFloat();
      tagData.weiFilled = true;
      Serial.print("Updated wei: "); Serial.println(val);
      break;
    case 5:
      tagData.lef = val.toFloat();
      tagData.lefFilled = true;
      Serial.print("Updated lef: "); Serial.println(val);
      break;
  }
}

bool isNumber(String str) {
  bool decimalPointSeen = false;
  int start = 0;

  if (str.length() == 0) return false;
  if (str[0] == '-' || str[0] == '+') start = 1;

  for (int i = start; i < (int)str.length(); i++) {
    if (isDigit(str[i])) continue;
    else if (str[i] == '.' && !decimalPointSeen) decimalPointSeen = true;
    else return false;
  }
  return true;
}
