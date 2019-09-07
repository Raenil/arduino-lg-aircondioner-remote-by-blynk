#define BLYNK_PRINT Serial
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <IRremote.h>

IRsend irsend;
byte arduino_mac[] = {};
IPAddress arduino_ip ();
IPAddress dns_ip     ();
IPAddress gateway_ip ();
IPAddress subnet_mask();
int ON = 1;
int OFF = 0;
int ONOFF;
int ONOFFSendCheck = 1;
int SendCheck = 1;
char auth[] = "";
int temperature;
int wind;
char mode;
String checksum = String(0, HEX);
String hextemp = String(0, HEX);
String hexwind = String(0, HEX);
String hexbase = String(8808, HEX);
String hexcode = String(0000000, HEX);
char tempT;
char windT;
char checksumT;
void setup()
{
  Serial.begin(9600);
  //Blynk.begin(auth);
  Blynk.begin(auth, "blynk-cloud.com", 80, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
}
//ONOFF
BLYNK_WRITE(V0)
{
  ONOFF = param.asInt();
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V0 =: ");
  Serial.println(ONOFF);
  ONOFFSendCheck = 0;
}
//Temperature
BLYNK_WRITE(V1)
{
  temperature = param.asInt();
  Serial.print("temperature = ");
  Serial.println(temperature);
  hextemp = String(temperature - 15, HEX);
  Serial.print("HexValue = ");
  Serial.println(hextemp);
  hextemp.toCharArray(tempT, 1);
  Serial.print("HexValueT = ");
  Serial.println(tempT);
  SendCheck = 0;
}
//Wind
BLYNK_WRITE(V2)
{
  wind = param.asInt();
  Serial.print("wind = ");
  Serial.println(wind);
  switch (wind)
  {
    case 1:
      hexwind = String(0, HEX);
      break;
    case 2:
      hexwind = String(2, HEX);
      break;
    case 3:
      hexwind = String(4, HEX);
      break;
  }
  Serial.print("hexwind = ");
  Serial.println(hexwind);
  SendCheck = 1;

}

void loop()
{
  Blynk.run();

  //ONOFF
  if (ONOFF && ONOFFSendCheck == 0)
  {
    irsend.sendLG(0x88C0051, 28);   //OFF
    delay (100);
    irsend.sendLG(0x88C0051, 28);   //OFF
    delay (100);
    irsend.sendLG(0x88C0051, 28);   //OFF
    Serial.println("Send On signal");
    ONOFFSendCheck = 1;
  }
  else if (ONOFF == 0 && ONOFFSendCheck == 0)
  {
    irsend.sendLG(0x8800909, 28);  //ON
    delay (100);
    irsend.sendLG(0x8800909, 28);  //ON
    delay (100);
    irsend.sendLG(0x8800909, 28);  //ON
    Serial.println("Send Off signal");
    ONOFFSendCheck = 1;
  }

  //Control
  if (SendCheck == 0)
  {
    int z2e;
    int o2e;
    int checksum;
    int hexchecksumint;
    String hexcode = String(0X0000000, HEX);
    String hexchecksum = String(0X00, HEX);;
    char checksumchar[1];
    char hexcodechar[1];
    hexcode = "8808" + hextemp + hexwind;
    Serial.print("hexcode = ");
    Serial.println(hexcode);
    for (z2e = 0, checksum = 0, o2e = 1; z2e < 6; z2e ++, o2e ++)
    {
      hexchecksum = hexcode.substring(z2e, o2e);
      Serial.print("hexchecksum(String) = ");
      Serial.println(hexchecksum);
      hexchecksum.toCharArray(checksumchar, hexchecksum.length() + 1);
      Serial.print("hexchecksumchar = ");
      Serial.println(checksumchar);
      checksum = checksum + strtoul(checksumchar, NULL, 16);
      Serial.print("checksum = ");
      Serial.println(checksum);
    }
    hexchecksum = String(checksum, HEX);
    Serial.print("checksumHEX = ");
    Serial.println(hexchecksum);
    hexchecksum = hexchecksum.substring(1, 2);
    Serial.print("checksumHEXtrim = ");
    Serial.println(hexchecksum);
    hexcode = hexcode + hexchecksum;
    Serial.print("hexcode = ");
    Serial.println(hexcode);
    hexcode.toCharArray(hexcodechar, hexcode.length() + 1);
    Serial.print("hexcodechar = ");
    Serial.println(hexcodechar);
    irsend.sendLG(hexcodechar, 28);
    SendCheck = 1;
  }
}
