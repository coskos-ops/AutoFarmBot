// to make this work -> no ground connection
// make sure both devices are connected to same computer
// Arduino -> ESP32
// TX -> RX (pin 16)
// RX -> TX (pin 17)

// next step is to communicate with robot
/*
void setup() {
  Serial.begin(9600); // Set the baud rate for communication with ESP32
}
int count = 0;
  int mod = 0;

void loop() {
  

  String messageToSend = "Harvest P: " + String(mod);
 
  count = count + 1;
  mod = count % 6;
  
  // Send the message
  Serial.println(messageToSend);

  // Add a delay before sending the next message (optional)
  delay(1000);
}

bool waitForAck() {
  // Wait for acknowledgment from ESP32
  while (Serial.available() > 0) {
    String ack = Serial.readStringUntil('\n');
    if (ack == "ACK") {
      return true;
    }
  }
  return false;
}*/

void setup() {
  Serial.begin(9600); // Set the baud rate for communication with ESP32
}
int count = 0;
  int mod = 0;

void loop() {
  // Check for incoming messages
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readStringUntil('\n');
    
    // Process the received message
    Serial.print("Received from Esp32: ");
    Serial.println(receivedMessage);
  }

  // String messageToSend = "Harvest P: " + String(mod);
 
  // count = count + 1;
  // mod = count % 6;
  
  // // Send the message
  // Serial.println(messageToSend);

  // Add a delay before sending the next message (optional)
  delay(1000);
}

bool waitForAck() {
  // Wait for acknowledgment from ESP32
  while (Serial.available() > 0) {
    String ack = Serial.readStringUntil('\n');
    if (ack == "ACK") {
      return true;
    }
  }
  return false;
}
