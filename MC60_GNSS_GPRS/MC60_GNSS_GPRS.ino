#define TINY_GSM_MODEM_MC60

#include <TinyGsmClient.h>

const char apn[]  = "ISAFE";
const char user[] = "";
const char pass[] = "";

#include <SoftwareSerial.h>
SoftwareSerial SerialAT(2, 3); // RX, TX

TinyGsm modem(SerialAT);

TinyGsmClient client(modem);

bool initializeGPS()
{
#ifdef TINY_GSM_MODEM_MC60
  Serial.println(F("Initializing GPS"));
  modem.gpsON();
#endif
}

bool initializeGSM()
{
  Serial.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.print(F("Modem: "));
  Serial.println(modemInfo);

#ifdef TINY_GSM_MODEM_BG96
  // Only for BG96
  modem.setGSMBand(GSM_ANY);
  delay(1000);
  modem.setMode(GSM_MODE);
  initializeGPS();
#endif

  // Serial.print("Set sleep: ");Serial.println(modem.sleepEnable(false));
  return true;
}

bool connectGPRS()
{
  Serial.print(F("Waiting for network..."));
  while (!modem.waitForNetwork(60000L)) {
    Serial.println(" fail");
    delay(10000);
  }
  Serial.println(" OK");

  Serial.print(F("Connecting to "));
  Serial.print(apn);

  modem.gprsConnect(apn, user, pass);
//  while (!modem.gprsConnect(apn, user, pass)) {
//    Serial.println(" fail");
//    delay(10000);
//  }
  Serial.println(" OK");
  Serial.println(modem.getLocalIP());
  return true;
}

void setup()
{
  Serial.begin(9600);
  SerialAT.begin(38400);
  initializeGSM();
  connectGPRS();
  initializeGPS();
}

void loop()
{

}
