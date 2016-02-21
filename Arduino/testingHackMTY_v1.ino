/**************************************************************************/
/*! 
    This example will wait for any ISO14443A card or tag, and
    depending on the size of the UID will attempt to read from it.
   
    If the card has a 4-byte UID it is probably a Mifare
    Classic card, and the following steps are taken:
   
    - Authenticate block 4 (the first block of Sector 1) using
      the default KEYA of 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF
    - If authentication succeeds, we can then read any of the
      4 blocks in that sector (though only block 4 is read here)
   
    If the card has a 7-byte UID it is probably a Mifare
    Ultralight card, and the 4 byte pages can be read directly.
    Page 4 is read by default since this is the first 'general-
    purpose' page on the tags.

    To enable debug message, define DEBUG in PN532/PN532_debug.h
*/
/**************************************************************************/
#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"

PN532_SPI pn532spi(SPI, 10);
PN532 nfc(pn532spi);
bool estacionado = false;
int matricula = 0;
int inByte = 0;

void setup(void) {
  //Bridge.begin();
  Serial.begin(9600);
  Serial.println("Hello!");
  delay(1000);

   pinMode(7, OUTPUT);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();

//  runCpuInfo();

  
  Serial.println("Waiting for an ISO14443A Card ...");
}
//--------------------------------------------------------------------
String PrintHexChar2(const uint8_t *data, const uint32_t numBytes){
  String cadena = "";
  #ifdef ARDUINO
    for (uint8_t i = 0; i < numBytes; i++) {
        char c = data[i];
        if (c <= 0x1f || c > 0x7f) {
            cadena = cadena + ".";
        } else {
            cadena = cadena + c;
        }
    }
    cadena + cadena + "";
    #else
    for (uint8_t i = 0; i < numBytes; i++) {
        cadena = cadena + data[i];
    }
    cadena = cadena + "    ";
    for (uint8_t i = 0; i < numBytes; i++) {
        char c = data[i];
        if (c <= 0x1f || c > 0x7f) {
            //printf(".");
            cadena = cadena + ".";}
        else {
            cadena = cadena + c;
        }
        cadena = cadena + "\n";
    }
#endif
return cadena ;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void loop(void) {

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    estacionado = true;
    String complete = "";
          //while(success){

    if (uidLength == 7) {
      uint8_t data[32];

      String key1 = "";
      String key2 = "";
      String key3 = "";
       
      success = nfc.mifareultralight_ReadPage(7, data);
      if (success) { key1 = PrintHexChar2(data, 4); }
        
      success = nfc.mifareultralight_ReadPage(8, data);
      if (success) { key2 = PrintHexChar2(data, 4); }
        
      success = nfc.mifareultralight_ReadPage(9, data);
      if (success) { key3 = PrintHexChar2(data, 4); }

      complete = key1.substring(2) + key2 + key3[0]+key3[1];
      while(success){
        if(complete.toInt()==87654321){
           digitalWrite(7, HIGH);
        }
        else{
          digitalWrite(7, LOW);
        }
        delay(800);
        digitalWrite(7, LOW);

        //if(Serial.available() > 0){
          //Serial.s
          //inByte = Serial.read(); //read the incoming byte
          //if(inByte == '1')
          //      digitalWrite(7, HIGH); //turn the LED on
          //else
          //      digitalWrite(7, LOW); //turn the LED off
          //}
        success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);}
      digitalWrite(7, LOW);
      matricula = complete.toInt();
      Serial.print("*" + complete + "*");
      Serial.println("");
      delay(800);
      }
                //success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);}

  }
  else{
    estacionado = false;
    digitalWrite(7, LOW);
  }
}

