#ifndef NFC_WRITER_H
#define NFC_WRITER_H

#include <Arduino.h>



void initNFC();
bool writeFilamentData(const String &materialType, const String &color, 
                       const String &spoolWeight, const String &tagName, 
                       float spoolWeightValue);

// bool readFilamentData(String &materialType, String &color, 
//                       String &spoolWeight, String &tagName, 
//                       float &spoolWeightValue, float &filamentAdjustLeftTEMP, long &pulses);

bool isTagPresentCustom();




#endif
