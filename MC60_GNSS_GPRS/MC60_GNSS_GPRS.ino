#include "OzHttpClient.h"
#include <ArduinoJson.h>
#define TINY_GSM_MODEM_MC60
#include <TinyGsmClient.h>
const char apn[]  = "ISAFE";
const char user[] = "";
const char pass[] = "";

//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX

#define SerialAT Serial3// RX, TX

//TinyGsm modem(SerialAT);
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);

TinyGsmClient client(modem);

// Variable for http request
OzHttpClient http;
http_request_t request;
http_response_t response;

bool initializeGPS()
{
#ifdef TINY_GSM_MODEM_MC60
  Serial.println(F("Initializing GPS"));
  // modem.gpsON();
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
  while (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);
  }
  Serial.println(" OK");
  Serial.println(modem.getLocalIP());
  return true;
}

void setup()
{
  Serial.begin(9600);
  SerialAT.begin(9600);
  initializeGSM();
  connectGPRS();

  delay(2000);

  initializeGPS();
}

void loop()
{
  bool status = false;

  StaticJsonDocument<100> jsonBuffer;
  JsonObject payload = jsonBuffer.to<JsonObject>();

  payload["t"] = 34;
  payload["lat"] = 23.56;
  payload["lon"] = 73.77;

  serializeJson(payload, Serial);
  Serial.println("");

  request.hostname = String("shridatt.eu-gb.mybluemix.net");
  request.port = 80;
  String path = "/gpsdata";
  request.path = path;

  serializeJson(payload, request.body);

  http.post(client, request, response);

  if (response.status == 200)
  {
    status = true;
  }
  else {
    status = false;
  }
  return status;

}
