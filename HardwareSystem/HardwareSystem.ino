#include "SR04.h"
#include "WiFiCredentials.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const int NUM_SENSORS = 3;
const int PH_SAMPLES = 10;
//DEFINE PINOUTS
const int TRIG_PIN = 12;
const int ECHO_PIN = 14;
const int PO_PIN = 33;
const int TDS_PIN = 32;
const int TEMP_PIN = 35;

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);

//DEFINE NETWORK
const char* ssid = (const char*) MY_SSID;
const char* password = (const char*) MY_PASSWORD;
String serverName = "https://coskos.pythonanywhere.com/postSensors";

void setup() {
  Serial.begin(115200); 
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

float get_pH(int pinout, int samples) {
  int measurings = 0;
  for (int i = 0; i < samples; i++){
    measurings += analogRead(pinout);
    delay(10);
  }
  return ((measurings/samples/4096)*14) - 1.2;
}

int get_TDS(int pinout) {
  return analogRead(pinout);
}

float get_Temp(int pinout) {
  return analogRead(pinout)*0.48828125;
}

void loop() {
  //Get Distance Sensor Data
  long distance = sr04.Distance();
  //Get pH Sensor Data
  float pH = get_pH(PO_PIN, PH_SAMPLES);
  //Get EC Sensor Data
  int tds = get_TDS(TDS_PIN);
  //Get Temperature Sensor Data
  float temperature = get_Temp(TEMP_PIN);
  //EC [mS/cm^2] = TDS/500
  float ec = tds/500.0;
  //Temperature Compensation
  //for every degree above 25C the TDS will be 2% larger (compounded)
  int tempApprx = round(temperature);
  if (tempApprx > 25) {
    while (tempApprx-- > 25) {
      ec *= 0.98;
    }
  }
  else if (tempApprx < 25) {
    while (tempApprx++ < 25) {
      ec *= 1.02;
    }
  }

  HTTPClient http;
  http.begin(serverName); 
  http.addHeader("Content-Type", "application/json");
  // http.addHeader("Accept", "*//*");
  // http.addHeader("User-Agend", "Content-Type: application/json);
  const int capacity = JSON_OBJECT_SIZE(NUM_SENSORS);
  DynamicJsonDocument doc(capacity);

  doc["distance"] = distance;
  doc["pH"] = pH;
  doc["ec"] = ec;
  String json;
  serializeJson(doc, json);
  Serial.println(json);

  int httpResponseCode = http.POST(json); //Send the actual POST request
  Serial.println(http.getString());
  http.end();
    delay(500);
}
