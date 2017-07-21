#include <OneWire.h>
#include "SimpleBLE.h"

int DS18S20_Pin = 2;
OneWire ds(DS18S20_Pin);

SimpleBLE ble;
String beaconMsg = "ESP00";
int rec=0;


void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
}

void loop() {
    float temperature = getTemp();
    Serial.println(temperature);
    if(temperature==-1000)
    {
      if(rec>5)
      {
        beaconMsg = "ESPSNA";
        Serial.println(beaconMsg);
        ble.begin(beaconMsg);
      }
    }
    else
    {
      beaconMsg = "ESP"+String(temperature);
      Serial.println(beaconMsg);
      ble.begin(beaconMsg);
    }
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      Serial.println("no addr found");
      rec++;
      Serial.println("rec = "+ rec);
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      rec++;
      Serial.println("rec = "+ rec);
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      rec++;
      Serial.println("rec = "+ rec);
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  Serial.println("rec = 0");
  rec=0;  
  return TemperatureSum;
  
}
