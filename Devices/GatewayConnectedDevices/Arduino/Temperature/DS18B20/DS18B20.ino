/* 
 Copyright (c) Neal Analytics, Inc.  All rights reserved.
 
 The MIT License (MIT)

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 

 Based upon code from Bildr.org at http://bildr.org/2011/07/ds18b20-arduino/ , which is copyright (c) 2010 bildr community, and licensed under the MIT license.
 
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Arduino code to read data from a DS18B20 temperature sensor, then augment and format as JSON to send via serial connection.
 Example of sending temperature data to Microsoft Azure and analyzing with Azure Stream Analytics or Azure Machine Learning.
 Real time output viewable at http://connectthedots.msopentech.com .
*/

// Constants used to add self-describing fields to the data before sending to Azure
char SensorSubject[] = "wthr";      // determines how Azure website will chart the data
char DeviceDisplayName[] = "Digital temp sensor";    // will be the label for the curve on the chart
char DeviceGUID[] = "2150719D-0FFF-4312-B61C-75AD5219D8FF";    // ensures all the data from this sensor appears on the same chart. You can use the Tools/Create GUID in Visual Studio to create.
char SensorDataType[]="temp";   // describes what is being sent to the gateway. Interpreted by the website.
char *SensorType; //describes model of sensor

#include <OneWire.h> 

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  float temperature = getTemp();
  Serial.print("{");
  Serial.print("\"dspl\":");
  Serial.print("\"");
  Serial.print(DeviceDisplayName);
  Serial.print("\"");
  Serial.print(",\"Subject\":");
  Serial.print("\"");
  Serial.print(SensorSubject);
  Serial.print("\"");
  Serial.print(",\"DeviceGUID\":");
  Serial.print("\"");
  Serial.print(DeviceGUID);
  Serial.print("\"");
  Serial.print(",\"Device\":");
  Serial.print("\"");
  Serial.print(SensorType);
  Serial.print("\"");
  Serial.print(",\"");
  Serial.print(SensorDataType);
  Serial.print("\":");
  Serial.print(temperature);
  Serial.println("}");
  delay(100); //just here to slow down the output so it is easier to read
  
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Fahrenheit

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] == 0x10 ) {
      SensorType= "DS18S20";
  }
  else if (addr[0] == 0x28){
      SensorType= "DS18BS20";
  }
  else
  {
      Serial.print("Device is not recognized");
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
  float Ftemp = (TemperatureSum * 9.0)/ 5.0 + 32.0;  //convert to Fahrenheit
  
  
  return Ftemp;
  
}
