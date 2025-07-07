#ifndef NFC_READER_H
#define NFC_READER_H

#include <Arduino.h>
#include <PN532.h>
#include <PN532_I2C.h>
#include <NfcAdapter.h>  // Needed for NfcTag

extern PN532_I2C pn532_i2c; // extern to writer
extern PN532 pn532;
extern NfcAdapter nfc;

struct TagData {
  String mat;
  String col;
  String spo;
  String tag;
  float wei;
  float lef;

  bool matFilled;
  bool colFilled;
  bool spoFilled;
  bool tagFilled;
  bool weiFilled;
  bool lefFilled;

  TagData();          // Constructor (will define in .cpp)
  bool isComplete();  // Declare only, define in .cpp
  void reset();       // Declare only, define in .cpp
};

// Global variable declaration
extern TagData tagData;

// Function declarations
bool readAndParseNFC();
String getTextRecordPayload(NfcTag &tag);
void parseAndUpdateTagData(String json);
bool fieldFilled(int index);
bool isStringField(int index);
void updateField(int index, String val);
bool isNumber(String str);

#endif
