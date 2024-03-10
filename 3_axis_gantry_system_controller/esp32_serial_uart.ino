#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define LED_BUILTIN 2

//DEFINE NETWORK
const char* ssid = (const char*) "BELL217"; 
const char* password = (const char*) "15AC5D3929E4";
const char* host = "coskos.pythonanywhere.com";
uint16_t httpsPort = 443;
String receivedMessage;
bool received = false;
bool verbose = false;

void setup() {
   pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
    delay(3000);
    if (verbose) Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
  
  Serial2.begin(9600); // Set the baud rate to match Arduino Uno
  if (verbose) Serial.begin(115200); // Initialize the default serial port for debugging

  WiFi.begin(ssid, password);
  if (verbose) Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    if (verbose) Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (verbose) Serial.println("");
  if (verbose) Serial.print("Connected to WiFi network with IP Address: ");
  if (verbose) Serial.println(WiFi.localIP());

  Serial2.flush();
  while(Serial2.available() > 0){
    Serial2.read();
  }
 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(10000);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  while(1){
  // Your additional code here
  WiFiClient client;

  // Connect to the server
  if (client.connect(host, httpsPort)) {
    if (verbose) Serial.println("Connected to server");

    // Make a GET request
    HTTPClient http;
    http.begin(client, "http://coskos.pythonanywhere.com/getCommand");
    int httpCode = http.GET();

    if (httpCode > 0) {
      if (verbose) Serial.printf("HTTP status code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        if (verbose) Serial.println("Received JSON response:");
        if (verbose) Serial.println(payload); //based on contents of payload this is where we tell the Arduino

        // Parse JSON using ArduinoJson
        DynamicJsonDocument jsonDocument(1024);  // Adjust the size based on your JSON
        deserializeJson(jsonDocument, payload);

        // Access JSON data
        const int id = jsonDocument["id"]; 
        const int target = jsonDocument["target"];
        const char* action = jsonDocument["action"];
        Serial.printf("Command received: \nId: %d  Target: %d  Action: %s \n", id, target, action);
        char command [100];
        sprintf(command, "%d,%d,%s", id, target, action); 
        Serial2.println(command);

        // robot ack

        while (Serial2.available() <= 0) {  
          //Serial.print("Waiting"); 
        }
        
          receivedMessage = Serial2.readStringUntil('\n');
          if (verbose) Serial.print("Data Received:");
          if (verbose) Serial.println(receivedMessage);

          int commaIndex = receivedMessage.indexOf(',');
          String comm_id = receivedMessage.substring(0, commaIndex); // parse first word and remove whitespace
          if(comm_id == "Done") {
            received = true;
          } 
      
        if (received){
        int commaIndex = 0;
        int id_tx;
        commaIndex = receivedMessage.indexOf(',');
        String comm_id = receivedMessage.substring(commaIndex + 1); // parse last word and remove whitespace
        id_tx = comm_id.toInt();

        // Make a POST request with JSON data
        http.begin(client, "http://coskos.pythonanywhere.com/ackCommand");
        http.addHeader("Content-Type", "application/json");

        // Create a JSON object for the POST request
        DynamicJsonDocument postJson(1024);
        postJson["id"] = id_tx;

        // Convert the JSON object to a string
        String postPayload;
        serializeJson(postJson, postPayload);

        // Make the POST request
        int postCode = http.POST(postPayload);

        if (postCode > 0) {
          Serial.printf("POST HTTP status code: %d\n", postCode);

          if (postCode == HTTP_CODE_OK) {
            String postResponse = http.getString();
            if (verbose) Serial.println("POST Response:");
            if (verbose) Serial.println(postResponse);
          }
        } else {
          Serial.printf("POST HTTP request failed with error code: %s\n", http.errorToString(postCode).c_str());
        }
        received = false;

      }


      } else if (httpCode == 204){ //replace 204
        // no command ready
      }
    } else {
      Serial.printf("HTTP request failed with error code: %s\n", http.errorToString(httpCode).c_str());

    }

    // Close the connection
    http.end();
    client.stop();
    if (verbose) Serial.println("Connection closed");
  } else {
    if (verbose) Serial.println("Unable to connect to server");
  }

  // Wait before making the next request
  delay(5000);
  }
}
