/*
#define RXD0 3
#define TXD0 1

#define RXD2 16
#define TXD2 17

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Start serial communication at 9600 bps
  //Serial.begin(115200); // Start serial communication at 9600 bps
   Serial.println("works?");
  //Serial1.begin(9600, SERIAL_8N1, RXD0, TXD0);
  //Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
   Serial2.begin(9600);
   Serial.println("works 2");
 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println("works?");
  
   Serial.print("Data Received:");
   Serial.println(Serial2.readString());
   Serial2.flush();
  //clearSerial();
   // delay(1000);
  
   
  if (Serial2.available() > 0) {
    //int data = Serial2.read(); //Serial.readString(); // Read data from Arduino
    //Serial.println("Received data: " + (char) data); // Print received data
    Serial.print("Data Received:");
    Serial.println(Serial2.readString());
    Serial2.flush();
    //clearSerial();
    //int pos = data.indexOf('\n', 0);
    //String token = data.substring(pos);
    //Serial.println("Token: " + token); // Print received data
    //Serial.println("works?");
  }
  
}

void clearSerial() {
  while (Serial2.available() > 0) {
    Serial2.read(); // Read and discard any available data
  }
} */
/*
void setup() {
  Serial.begin(9600); // Set the baud rate to match the Arduino Uno
  //SerialUSB.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readString();
    Serial.println("Received: " + data);
    //SerialUSB.print("Received: " + data);
  }
} */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//DEFINE NETWORK
const char* ssid = (const char*) "BELL217"; 
const char* password = (const char*) "15AC5D3929E4";
const char* host = "coskos.pythonanywhere.com";
uint16_t httpsPort = 443;

void setup() {
  Serial2.begin(9600); // Set the baud rate to match Arduino Uno
  Serial.begin(115200); // Initialize the default serial port for debugging

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
  // Check for incoming messages
  // if (Serial2.available() > 0) {
  //   String receivedMessage = Serial2.readStringUntil('\n');
    
  //   // Process the received message
  //   Serial.print("Received from Arduino Uno: ");
  //   Serial.println(receivedMessage);

  // }

  // Your additional code here
  WiFiClient client;

  // Connect to the server
  if (client.connect(host, httpsPort)) {
    Serial.println("Connected to server");

    // Make a GET request
    HTTPClient http;
    http.begin(client, "http://coskos.pythonanywhere.com/getCommand");
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("HTTP status code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Received JSON response:");
        Serial.println(payload); //based on contents of payload this is where we tell the Arduino

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
      } else if (httpCode == 204){ //replace 204
        // no command ready
      }
    } else {
      Serial.printf("HTTP request failed with error code: %s\n", http.errorToString(httpCode).c_str());
    }

    // Close the connection
    http.end();
    client.stop();
    Serial.println("Connection closed");
  } else {
    Serial.println("Unable to connect to server");
  }

  // Wait before making the next request
  delay(5000);
}
