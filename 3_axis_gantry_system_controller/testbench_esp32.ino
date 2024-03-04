void setup() {
  // put your setup code here, to run once:
  Serial2.begin(9600); // Set the baud rate to match Arduino Uno
  Serial.begin(115200); // Initialize the default serial port for debugging
}

void loop() {
  // put your main code here, to run repeatedly:
  int id = 304;
  int target = 2;
  char* action = "harvest";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  char command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  id = 305;
  target = 1;
  action = "plant";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  id = 306;
  target = 3;
  action = "harvest";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  id = 307;
  target = 6;
  action = "harvest";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  id = 308;
  target = 4;
  action = "plant";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  id = 309;
  target = 5;
  action = "harvest";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  id = 310;
  target = 2;
  action = "plant";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  id = 311;
  target = 2;
  action = "plant";
  //Serial.printf("Command Sent: \nId: %d  Target: %d  Action: %s \n", id, target, action);
  command [100];
  sprintf(command, "%d,%d,%s", id, target, action); 
  Serial2.println(command);
  //delay(1000);

  if (Serial2.available() > 0) {
     String receivedMessage = Serial2.readStringUntil('\n');
    
     // Process the received message
     Serial.print("Received from Arduino Uno: ");
     Serial.println(receivedMessage);

  }
}
