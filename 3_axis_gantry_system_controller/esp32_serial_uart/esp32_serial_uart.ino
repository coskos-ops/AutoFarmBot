#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


#define LED_BUILTIN 2

const char* ssid = "Capstone";
const char* password = "xkkv1511";
const char* host = "coskos.pythonanywhere.com";
uint16_t httpsPort = 443;
bool verbose = false;

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
  int spaceIndex = distMessage.indexOf(' ');
  if (spaceIndex != -1) {
    String paramsSubstring = distMessage.substring(spaceIndex + 1);
    int distances[6]; // Array to store distances d1 to d6

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

    // Create JSON object for the POST request
    DynamicJsonDocument postJson(256); // Adjust size as needed
    postJson["d1"] = distances[0];
    postJson["d2"] = distances[1];
    postJson["d3"] = distances[2];
    postJson["d4"] = distances[3];
    postJson["d5"] = distances[4];
    postJson["d6"] = distances[5];

    // Convert JSON object to a string
    String postPayload;
    serializeJson(postJson, postPayload);

    // Make the POST request
    WiFiClient client;
    if (client.connect(host, httpsPort)) {
      HTTPClient http;
      http.begin(client, "http://coskos.pythonanywhere.com/postSensors");
      http.addHeader("Content-Type", "application/json");
      int postCode = http.POST(postPayload);

      if (postCode > 0 && postCode == HTTP_CODE_OK) {
        Serial.printf("POST HTTP status code: %d\n", postCode);
        String postResponse = http.getString();
        Serial.println("POST Response:");
        Serial.println(postResponse);
      } else {
        Serial.printf("POST HTTP request failed with error code: %s\n", http.errorToString(postCode).c_str());
      }
      http.end();
      client.stop();
    } else {
      Serial.println("Unable to connect to server for POST request");
    }
  } else {
    Serial.println("Invalid DIST message format");
  }
}

void loop() {
  while (1) {
    // Check if a "DIST" message has been received
    if (Serial2.available() > 0) {
      String distMessage = Serial2.readStringUntil('\n');
      if (distMessage.startsWith("DIST")) {
        // Parse and process the "DIST" message
        processDistMessage(distMessage);
      }
    }

    // If no "DIST" message received, proceed to get command from server
    else {
      WiFiClient client;
      if (client.connect(host, httpsPort)) {
        HTTPClient http;
        http.begin(client, "http://coskos.pythonanywhere.com/getCommand");
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
            String receivedMessage = Serial2.readStringUntil('\n');
            int commaIndex = receivedMessage.indexOf(',');
            String comm_id = receivedMessage.substring(0, commaIndex);
            if (comm_id == "Done") {
              int id_tx = receivedMessage.substring(commaIndex + 1).toInt();
              HTTPClient postHttp;
              postHttp.begin(client, "http://coskos.pythonanywhere.com/ackCommand");
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
        client.stop();
      } else {
        if (verbose) Serial.println("Unable to connect to server");
      }
    }

    delay(5000); // Delay before repeating the loop
  }
}
