#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


#define LED_BUILTIN 2

const char* ssid = "Capstone";
const char* password = "xkkv1511";
const char* host = "coskos.pythonanywhere.com";
uint16_t httpsPort = 443;
bool verbose = false;

const long scanDelay = 60000; // will check all plants for maturity (in ms)
unsigned long currentTime = 0;
unsigned long previousTime = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  if (verbose) Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(500);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void processDistMessage(String distMessage) {
  // Extract distances from the "DIST" message
    String paramsSubstring = distMessage;
    long distances[6]; // Array to store distances d1 to d6

    for (int i = 0; i < 6; ++i) {
      int nextSpace = paramsSubstring.indexOf(' ');
      if (nextSpace != -1) {
        String param = paramsSubstring.substring(0, nextSpace);
        distances[i] = param.toInt();
        paramsSubstring = paramsSubstring.substring(nextSpace + 1); // Move to the next distance
      } else {
        // Last distance
        distances[i] = paramsSubstring.toInt();
      }
    }
    const int capacity = JSON_OBJECT_SIZE(6);
    DynamicJsonDocument doc(capacity);
    // Create JSON object for the POST request
    doc["d1"] = distances[0];
    doc["d2"] = distances[1];
    doc["d3"] = distances[2];
    doc["d4"] = distances[3];
    doc["d5"] = distances[4];
    doc["d6"] = distances[5];

    // Convert JSON object to a string
    String postPayload;
    serializeJson(doc, postPayload);

    // Make the POST request
    //WiFiClient client;
    HTTPClient http;
    http.begin("https://coskos.pythonanywhere.com/postSensors");
    http.addHeader("Content-Type", "application/json");
    int postCode = http.POST(postPayload);
    Serial.print(http.getString());
    http.end();
  
}

void loop() {

    HTTPClient http;
    http.begin("https://coskos.pythonanywhere.com/getCommand");
    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument jsonDocument(1024);
        deserializeJson(jsonDocument, payload);
        const int id = jsonDocument["id"];
        const int target = jsonDocument["target"];
        const char* action = jsonDocument["action"];
        Serial.printf("Command received: Id: %d  Target: %d  Action: %s \n", id, target, action);
        char command[100];
        sprintf(command, "%d,%d,%s", id, target, action);
        Serial2.println(command);

        // Wait for acknowledgment from other device
        while (Serial2.available() <= 0);
        Serial.printf("Waiting for GET...");
        String receivedMessage = Serial2.readStringUntil('\n');
        Serial.printf("Done\n");
        int commaIndex = receivedMessage.indexOf(',');
        String comm_id = receivedMessage.substring(0, commaIndex);
        if (comm_id == "Done") {
          int id_tx = receivedMessage.substring(commaIndex + 1).toInt();
          HTTPClient postHttp;
          postHttp.begin("https://coskos.pythonanywhere.com/ackCommand");
          postHttp.addHeader("Content-Type", "application/json");
          DynamicJsonDocument postJson(1024);
          postJson["id"] = id_tx;
          String postPayload;
          serializeJson(postJson, postPayload);
          int postCode = postHttp.POST(postPayload);
          if (postCode > 0 && postCode == HTTP_CODE_OK) {
            Serial.printf("POST HTTP status code: %d\n", postCode);
            String postResponse = postHttp.getString();
            if (verbose) Serial.println("POST Response:");
            if (verbose) Serial.println(postResponse);
          } else {
            Serial.printf("POST HTTP request failed with error code: %s\n", postHttp.errorToString(postCode).c_str());
          }
        }
      } else if (httpCode == 204) {
        // no command ready. Check distance from 
      }
    } else {
      Serial.printf("HTTP request failed with error code: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();


    currentTime = millis(); // in milliseconds

    // is it time yet?
    // if(verbose) Serial.print("\nCurrent: ");
    // if(verbose) Serial.print(currentTime);
    // if(verbose) Serial.print("\nPrevious: ");
    // if(verbose) Serial.print(previousTime);
    // if(verbose) Serial.print("\nScan: ");
    // if(verbose) Serial.print(scanDelay);
    // if(verbose) Serial.print("\nMinus: ");
    // if(verbose) Serial.print(currentTime - previousTime);
    Serial.print("Current time ");
    Serial.println(currentTime);
    Serial.print("previousTime ");
    Serial.println(previousTime);
    if(currentTime - previousTime >= scanDelay){
      // code to scan
      Serial.print("Gantry Scanning to check for Plant Maturity\n");
      Serial2.println("DIST");

      while (Serial2.available() <= 0);
      String receivedMessage = Serial2.readStringUntil('\n');
      Serial.print("Recieved: ");
      Serial.println(receivedMessage);
      processDistMessage(receivedMessage);
      previousTime = currentTime;
    }





  delay(1000); // Delay before repeating the loop
}
