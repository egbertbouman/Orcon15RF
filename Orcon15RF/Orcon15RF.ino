#include "ELECHOUSE_CC1101_SRC_DRV.h"
#include <WiFiManager.h>

byte buffer[256] = {0};
byte valid_packet[] = {0xb3, 0x2a, 0xab, 0x2a, 0x95};

void setup(){
    Serial.begin(115200);
    if (!ELECHOUSE_cc1101.getCC1101()){
        Serial.println("Connection Error");
    }

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setCCMode(1);
    ELECHOUSE_cc1101.setModulation(0);
    ELECHOUSE_cc1101.setClb(3, 0, 0);
    ELECHOUSE_cc1101.setMHZ(868.3);
    ELECHOUSE_cc1101.setDeviation(47.60);
    ELECHOUSE_cc1101.setChannel(0);
    ELECHOUSE_cc1101.setChsp(199.95);
    ELECHOUSE_cc1101.setRxBW(325);
    ELECHOUSE_cc1101.setDRate(38.46);
    ELECHOUSE_cc1101.setPA(10);
    ELECHOUSE_cc1101.setSyncMode(2);
    ELECHOUSE_cc1101.setSyncWord(0xaa,0xab);
    ELECHOUSE_cc1101.setAdrChk(0);
    ELECHOUSE_cc1101.setAddr(0);
    ELECHOUSE_cc1101.setWhiteData(0);
    ELECHOUSE_cc1101.setPktFormat(0);
    ELECHOUSE_cc1101.setLengthConfig(0);
    ELECHOUSE_cc1101.setCrc(0);
    ELECHOUSE_cc1101.setCRC_AF(0);
    ELECHOUSE_cc1101.setDcFilterOff(0);
    ELECHOUSE_cc1101.setManchester(0);
    ELECHOUSE_cc1101.setFEC(0);
    ELECHOUSE_cc1101.setPRE(0);
    ELECHOUSE_cc1101.setPQT(0);
    ELECHOUSE_cc1101.setAppendStatus(0);

    ELECHOUSE_cc1101.SetRx();
    ELECHOUSE_cc1101.setPacketLength(255);

    WiFi.mode(WIFI_OFF);
}

void loop(){
    char out[2];
    String cmd;
    Serial.setTimeout(100);
    cmd = Serial.readStringUntil('\n');
    if (cmd != ""){
        ELECHOUSE_cc1101.SetTx();
        ELECHOUSE_cc1101.setPacketLength(cmd.length()/2);
        
        hexToBytes(buffer, cmd);
        ELECHOUSE_cc1101.SendData(buffer, cmd.length()/2);

        ELECHOUSE_cc1101.SetRx();
        ELECHOUSE_cc1101.setPacketLength(255);
    }
    
    if (ELECHOUSE_cc1101.CheckRxFifo(1000)){
        int len = ELECHOUSE_cc1101.ReceiveData(buffer);

        //Drop packets that are too small or that don't have the right prefix
        if (len < sizeof(valid_packet)){ return; }
        for(int i = 0; i < sizeof(valid_packet); i++){
            if(buffer[i+1] != valid_packet[i]){ return; }
        }

        //Print received bytes in hex.
        for (int i = 0; i<len; i++){
            sprintf(out, "%.2X", buffer[i]);
            Serial.print(out);
        }
        Serial.println();
    }
}

byte hexToBytes(byte *byteArray, String hexString){
  byte currentByte = 0;
  byte byteIndex = 0;
  bool odd = hexString.length() & 1;
  
  for (byte charIndex = 0; charIndex < hexString.length(); charIndex++){
    if ((charIndex & 1) == odd){
      currentByte = charToByte(hexString[charIndex]) << 4;
    }
    else{
      currentByte |= charToByte(hexString[charIndex]);
      byteArray[byteIndex++] = currentByte;
      currentByte = 0;
    }
  }
  return byteIndex;
}

byte charToByte(char c){
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0;
}
