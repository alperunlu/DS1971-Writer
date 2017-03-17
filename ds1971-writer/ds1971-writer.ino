//
//  Maxim Dallas DS1971-F5 IButton Memory Filler
//  Fills IButton's EEPROM with a hardcoded byte. (default : FF)
//  Attach the 5V through 4.7k ohm resistor to pin 7, and connect GND.
//  Written by Alper Unlu & Okan Erinc Yildirim

#include <OneWire.h>
OneWire ds(7); //pin 7
bool buttonaktif = false;
byte findAddr[8];
byte addr[8];
byte i;
byte data[512];
byte present = 0;
byte nextCmd = 0;

#define CMD_NULL 0x00
#define CMD_CHECK_DEVICE 0x02

void setup(void) {
memset(addr, 0, 8);
memset(data, 0, 512);
nextCmd = CMD_NULL;
Serial.begin(9600);
Serial.print("Hazir, IButton bekleniyor... \n");
}

bool bul()
{
ds.reset_search();
delay(250);
if ( !ds.search(findAddr)) {
return false;
}
return true;
}

void info(byte addr[])
{
Serial.print("ADDR: ");
for ( i = 0; i < 8; i++) {
Serial.print(addr[i], HEX);
if(i<7) Serial.print(":");
}
Serial.print("\n");
}

void sendCMD(int cmd)
{
present = ds.reset();
ds.select(addr);
ds.write(cmd,1); // Scratchpad'i oku
delay(2000);
memset(data, 0, 512);
}

bool memOku(byte type,byte pos=0x00, byte _size=0x01)
{
sendCMD(type);
ds.write(pos);
Serial.print("|");
for ( i = 0; i < _size; i++) {
data[i] = ds.read();
Serial.print(data[i], HEX);
Serial.print("|");
}
Serial.print("\n");
ds.reset();
delay(1000);
return true;
}

bool vaziyet()
{
if (!bul()) return true;
if (memcmp(findAddr, addr, 8) == 0) return false;
return true;
}

void OkuYaz()
{
 int i;
 ds.reset();
 ds.select(addr);
 ds.write(0x0F,1);  // Scratchpad'e yaz
 ds.write(0x00,1);
 for ( i = 0; i < 32; i++) {
 ds.write(0xFF,1); //Memory'ye yazmak istediğimiz byte 
 }  
 delay(250);
 ds.reset();
 ds.select(addr);
 ds.write(0x55,1);  // Scratchpad'i Memory'e kopyala
 ds.write(0xA5,1);
 delay(250); 
 
}
void memoryVer()
{
OkuYaz();
Serial.print("-----------------------------------------------------------------------\n");
Serial.print("SCRATCHPAD: ");
memOku(0xaa,0,32); //Scratchpad'i oku
Serial.print("MEMORY: ");
memOku(0xf0 ,0,32); //Memory oku
Serial.print("APPLICATION: ");
memOku(0xc3,0,8); //Application Register'ları oku
Serial.print("-----------------------------------------------------------------------\n");

}

void loop(void) {
if (!buttonaktif)
{
nextCmd = CMD_NULL;

if (!bul()) {
buttonaktif = false;
Serial.print("Ariyor ... \n");
delay(500);
return;
}
memcpy(addr, findAddr, 8);
if ( OneWire::crc8( addr, 7) != addr[7]) {
buttonaktif = false;
return;
};
Serial.print("Baglandi .... \n");
buttonaktif = true;
info(addr);
nextCmd = CMD_CHECK_DEVICE;
} 

switch(nextCmd)
{
case CMD_CHECK_DEVICE:
Serial.print("Lutfen Bekleyin: Hafiza dokumu aliniyor... \n");
memoryVer();
nextCmd = CMD_NULL;
break;
}

if (vaziyet()) {
buttonaktif = false;
Serial.print("IButton bagli degil .... \n");
return;
}
}




