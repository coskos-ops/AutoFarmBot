#include "SR04.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
const int TRIG_PIN = 12;
const int ECHO_PIN = 14;

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;


const char* ssid = "Kostic";
const char* password = "";
String serverName = "https://coskos.pythonanywhere.com/postDistance";
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

void loop() {
  long distance = sr04.Distance();
  HTTPClient http;
  http.begin(serverName); 
  http.addHeader("Content-Type", "application/json");
  // http.addHeader("Accept", "*//*");
  // http.addHeader("User-Agend", "Content-Type: application/json);
  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> doc;

  doc["distance"] = distance;
  String json;
  serializeJson(doc, json);
  Serial.println(json);




  

  int httpResponseCode = http.POST(json); //Send the actual POST request
  Serial.println(http.getString());


  http.end();



   delay(1000);
  //}
}
