#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <IRremote.h>
IRsend irsend;
//Blynk setting-----------------------------------------------------------------
byte arduino_mac[] = {};
IPAddress arduino_ip ();
IPAddress dns_ip     ();
IPAddress gateway_ip ();
IPAddress subnet_mask();
char auth[] = "";
//Blynk setting-----------------------------------------------------------------


const int ON = 1;
const int OFF = 0;
int ONOFF;
int ONOFFOLD;
int SendCheck = 1;
int temperature;
int wind;
String hextemp = String(0, HEX);
String hexwind = String(0, HEX);
String hexbase = String(8808, HEX);
int First = 0;

String hexcode = String(0X0000000, HEX);
String hexchecksum = String(0X00, HEX);
int z2e;
int o2e;
int checksum;
int hexchecksumint;
long hexcodelong;

char checksumchar[1] = "0";
char hexcodechar[1] = "0";

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, "blynk-cloud.com", 80, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
}

BLYNK_CONNECTED()
{
  Serial.println("BLYNK_CONNECTED()");
  Serial.println("Call V1");
  Blynk.syncVirtual(V1); //Call V1
  Serial.println("Call V2");
  Blynk.syncVirtual(V2); //Call V2
}


void hexcalculate()
{
  if (SendCheck == 0)
  {
    if (ONOFF != ONOFFOLD)
    {
      //ONOFF
      if (ONOFF)
      {
        irsend.sendLG(0x88C0051, 28);   //OFF
        delay (100);
        irsend.sendLG(0x88C0051, 28);   //OFF
        delay (100);
        irsend.sendLG(0x88C0051, 28);   //OFF
        Serial.println("Send On signal");
        SendCheck = 1;
        ONOFFOLD = 1;
      }
      else if (ONOFF == 0)
      {
        irsend.sendLG(0x8800909, 28);  //ON
        delay (100);
        irsend.sendLG(0x8800909, 28);  //ON
        delay (100);
        irsend.sendLG(0x8800909, 28);  //ON
        Serial.println("Send Off signal");
        SendCheck = 1;
        ONOFFOLD = 0;
      }
    }
    else
    {
      //temp
      Serial.print("temperature = ");
      Serial.println(temperature);
      hextemp = String(temperature - 15, HEX);
      Serial.print("HexValue = ");
      Serial.println(hextemp);

      //wind
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
      Serial.print("hexchecksum = ");
      Serial.println(hexchecksum);
      Serial.print("hexchecksum.length() = ");
      Serial.println(hexchecksum.length());
      hexchecksum = hexchecksum.substring(hexchecksum.length() - 1, hexchecksum.length());
      Serial.print("checksumHEXtrim = ");
      Serial.println(hexchecksum);
      hexcode = hexcode + hexchecksum;
      Serial.print("hexcode = ");
      Serial.println(hexcode);
      hexcode.toCharArray(hexcodechar, hexcode.length() + 1);
      Serial.print("hexcodechar = ");
      Serial.println(hexcodechar);
      hexcodelong = strtoul(hexcodechar, NULL, 16);
      Serial.print("hexcodelong = ");
      Serial.println(hexcodelong);
      irsend.sendLG(hexcodelong, 28);
      SendCheck = 1;
    }
  }
}

//ONOFF
BLYNK_WRITE(V0)
{
  ONOFF = param.asInt();
  Serial.print("V0 =: ");
  Serial.println(ONOFF);
  SendCheck = 0;
}

//Temperature
BLYNK_WRITE(V1)
{
  Serial.println("BLYNK_WRITE(V1)");
  temperature = param.asInt();
  if (First < 2)
  {
    Serial.println("Firsttemp");
    First ++;
  }
  else
  {
    SendCheck = 0;
  }
}

//Wind
BLYNK_WRITE(V2)
{
  Serial.println("BLYNK_WRITE(V2)");
  wind = param.asInt();
  if (First < 2)
  {
    Serial.println("Firstwind");
    First ++;
  }
  else
  {
    SendCheck = 0;
  }
}

void hexsand()
{

}


void loop()
{
  Blynk.run();

  timer.setInterval(1000L, hexcalculate);
}
