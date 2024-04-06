#include <AccelStepper.h>
#include <Servo.h>

// Reference Table for Stepper Distances of each plant with respect to Collection bin (position 0)
// Assumed ordering (motor is refering to XY direction - written twice but the same):
// Motor    Plant 1     Plant 3     Plant 5
// Motor    Plant 2     Plant 4     Plant 6
/*
const int x1 = 600;
const int x2 = 1700;
const int x3 = 3000;
const int y1 = 600;
const int y2 = 1200;
*/

const int x1 = 75;
const int x2 = 725;
const int x3 = 1400;
const int y1 = 400;
const int y2 = 1150;
const int plant_z = 1625;

const int plant_1_x = x1; // theoretically determined for now
const int plant_1_y = y1; // need to confirm in person (Logan or Kris)
const int plant_2_x = x1; // thought of as a rectangular box
const int plant_2_y = y2;
const int plant_3_x = x2; // technically can be simplified to 3 X positions and 2 Y positions
const int plant_3_y = y1; // and only 1 Z position (assuming all have 0 as default position)
const int plant_4_x = x2; // wrote out all cases for now
const int plant_4_y = y2;
const int plant_5_x = x3;
const int plant_5_y = y1;
const int plant_6_x = x3;
const int plant_6_y = y2;
//const int plant_z = 1900; //location where cut occurs (2-3 cm from base)

// Motor pin configurations
const int X_step_pin = 2;
const int X_dir_pin = 5;
const int Y_step_pin = 3;
const int Y_dir_pin = 6;
const int Z_step_pin = 4;
const int Z_dir_pin = 7;
const int claw_pin = 13;

// User define steps per second for all motors
const int motorSpeed = 80;
const int maxSpeed = 500;
const int acceleration = 100;
const long scanDelay = 300000; // will check all plants for maturity (in ms)
//int locations [6] = {2,3,5,1,4,6}; for testing purposes

// debug mode
bool verbose = false;

// flags for communications
bool ready = true;
bool harvest = false; //signals from ESP
bool plant = false;

// for calculations
int plantID = 1; // values range from 1 to 6
int tx_rx_ID = 0;
String prevRecMessage = "empty";
unsigned long currentTime = 0;
unsigned long previousTime = 0;
int pos = 0;    // variable to store the servo position

AccelStepper stepperX(1, X_step_pin, X_dir_pin);
AccelStepper stepperY(1, Y_step_pin, Y_dir_pin);
AccelStepper stepperZ(1, Z_step_pin, Z_dir_pin);
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  pinMode(X_step_pin, OUTPUT);
  pinMode(X_dir_pin, OUTPUT);
  pinMode(Y_step_pin, OUTPUT);
  pinMode(Y_dir_pin, OUTPUT);
  pinMode(Z_step_pin, OUTPUT);
  pinMode(Z_dir_pin, OUTPUT);

  Serial.begin(9600); // for communications 
  
  myservo.attach(claw_pin);  // attaches the servo on pin 9 to the servo object
  
  stepperX.setMaxSpeed(maxSpeed);
  stepperY.setMaxSpeed(maxSpeed);
  stepperZ.setMaxSpeed(maxSpeed);

  stepperX.setAcceleration(acceleration);
  stepperY.setAcceleration(acceleration);
  stepperZ.setAcceleration(acceleration);

  stepperX.setSpeed(motorSpeed);
  stepperY.setSpeed(motorSpeed);
  stepperZ.setSpeed(motorSpeed);

  // calibration: ensure claw is above collection bin at the highest point
  // all operations should reset to this point
  stepperX.runToNewPosition(0);
  stepperY.runToNewPosition(0);
  stepperZ.runToNewPosition(0); 

  close_claw();
  // Ground the signal pin after operation finishes
  digitalWrite(claw_pin, LOW); // Ground the signal pin
  delay(1000); 
}

void loop() {
  //Serial.print("Hi\n");
  if(verbose) Serial.print("\nLoop: \n");

  /*
      Communications with ESP Controller (Primary)

      Receive: Plant/Harvest/Other operations, Plant ID
      Formatting (Relevant to Filip probably):
        <operation>,<plant #>
        Ex.1 plant,1
        Ex.2 harvest,6
      Note: newline character is important

      Tranmit: Busy/Ready
  */
  
  while(1){
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readStringUntil('\n');
    if(receivedMessage == prevRecMessage){
      //break;
      continue;
    }
    //String receivedMessage = "204,3,harvest";
    // Process the received message
    if (verbose) Serial.print("\nReceived from Esp32: ");
    if (verbose) Serial.println(receivedMessage);
    esp32Decoder(receivedMessage, &harvest, &plant, &ready, &plantID, &tx_rx_ID);

    if(!ready){
      if (verbose) Serial.write("Busy\n"); // inform esp32 that no instruction will be accepted
    }
    if(verbose) printDebug("Harvest ", harvest);
    if(verbose) printDebug("Plant ", plant);
    if(verbose) printDebug("Ready ", ready);
    if(verbose) printDebug("Plant Number ", plantID);
    if(verbose) printDebug("TX RX IDENTITY ", tx_rx_ID);

    
    //    Based on comms, translate into position and move 3D gantry
    if(harvest){
      if(verbose) Serial.print("Harvesting in progress \n");
      // Ground the signal pin after operation finishes
      digitalWrite(claw_pin, LOW); // Ground the signal pin
      delay(1000); 
      // go to plant
      stepperY.runToNewPosition(plantIDtoYposition(plantID));
      stepperX.runToNewPosition(plantIDtoXposition(plantID));
      // signal to servo motor (sg90) -> OPEN
      // Ground the signal pin after operation finishes
      digitalWrite(claw_pin, LOW); // Ground the signal pin
      delay(1000); 
      open_claw();
      stepperZ.runToNewPosition(plant_z);
      // Ground the signal pin after operation finishes
      digitalWrite(claw_pin, LOW); // Ground the signal pin
      delay(1000); 
      close_claw();
      
      //stepperZ.runToNewPosition(plant_z-500);

      // reset position
      stepperZ.runToNewPosition(0);
      stepperY.runToNewPosition(0);
      stepperX.runToNewPosition(0);
      // Ground the signal pin after operation finishes
      digitalWrite(claw_pin, LOW); // Ground the signal pin
      delay(1000); 

      // drop plant in container (TBD KRIS) 
      // signal to servo motor (sg90) -> OPEN
      open_claw();
      delay(2000);
      //close
      close_claw();
      
      // reset flags
      ready = true;
      harvest = false;
    } else if(plant){
      if(verbose) Serial.print("Planting in progress \n");
      
      // go to plant
      stepperY.runToNewPosition(plantIDtoYposition(plantID));
      stepperX.runToNewPosition(plantIDtoXposition(plantID));
      stepperZ.runToNewPosition(plant_z);
      stepperZ.runToNewPosition(plant_z-500);

      // plant seed (TBD KRIS or LOGAN)

      // reset position
      //stepperZ.runToNewPosition(0);
      //stepperY.runToNewPosition(0);
      //stepperX.runToNewPosition(0);
      
      // reset flags
      ready = true;
      plant = false;
    } else {
      // remain idle
      if(verbose) Serial.print("No task assigned, saving power \n");
    }

    if(ready){
      char ack_command [100];
      sprintf(ack_command, "Done,%d", tx_rx_ID); 
      Serial.println(ack_command);
    }
    
    //if(verbose) delay(300); // 0.3 second delay
    prevRecMessage = receivedMessage;
  }
    // code to check plant height
    currentTime = millis(); // in milliseconds

    // is it time yet?
    if(verbose) Serial.print("\nCurrent: ");
    if(verbose) Serial.print(currentTime);
    if(verbose) Serial.print("\nPrevious: ");
    if(verbose) Serial.print(previousTime);
    if(verbose) Serial.print("\nScan: ");
    if(verbose) Serial.print(scanDelay);
    if(verbose) Serial.print("\nMinus: ");
    if(verbose) Serial.print(currentTime - previousTime);
    if(currentTime - previousTime >= scanDelay){
      // code to scan
      if(verbose) Serial.print("Gantry Scanning to check for Plant Maturity\n");
      scan_for_plant_maturity();
      previousTime = currentTime;
    }
  }

  //reset();
  if(verbose) Serial.print("Program Closed\n");
  //exit(0);
}

void close_claw(){
  for (pos = 30; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  // Ground the signal pin after operation finishes
  digitalWrite(claw_pin, LOW); // Ground the signal pin
  delay(1000); 
}

void open_claw(){
  for (pos = 120; pos >= 30; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  // Ground the signal pin after operation finishes
  digitalWrite(claw_pin, LOW); // Ground the signal pin
  delay(1000); 
}

void close_claw_old(){
  for (pos = 50; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void open_claw_old(){
  for (pos = 120; pos >= 50; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void scan_for_plant_maturity(){
  stepperY.runToNewPosition(plantIDtoYposition(5));
  stepperX.runToNewPosition(plantIDtoXposition(5));
  //stepperZ.runToNewPosition(plant_z);
  stepperZ.runToNewPosition(plant_z-500);
  //delay(3000);
  stepperY.runToNewPosition(plantIDtoYposition(3));
  stepperX.runToNewPosition(plantIDtoXposition(3));
  //stepperZ.runToNewPosition(plant_z);
  stepperZ.runToNewPosition(plant_z-500);
  //delay(3000);
  stepperY.runToNewPosition(plantIDtoYposition(1));
  stepperX.runToNewPosition(plantIDtoXposition(1));
  //stepperZ.runToNewPosition(plant_z);
  stepperZ.runToNewPosition(plant_z-500);
  //delay(3000);
  stepperY.runToNewPosition(plantIDtoYposition(2));
  stepperX.runToNewPosition(plantIDtoXposition(2));
  //stepperZ.runToNewPosition(plant_z);
  stepperZ.runToNewPosition(plant_z-500);
  //delay(3000);
  stepperY.runToNewPosition(plantIDtoYposition(4));
  stepperX.runToNewPosition(plantIDtoXposition(4));
  //stepperZ.runToNewPosition(plant_z);
  stepperZ.runToNewPosition(plant_z-500);
  //delay(3000);
  stepperY.runToNewPosition(plantIDtoYposition(6));
  stepperX.runToNewPosition(plantIDtoXposition(6));
  //stepperZ.runToNewPosition(plant_z);
  stepperZ.runToNewPosition(plant_z-500);
  //delay(3000);
  stepperX.runToNewPosition(0);
  stepperY.runToNewPosition(0);
  stepperZ.runToNewPosition(0); 
}

void reset (){
  stepperZ.runToNewPosition(0);
  stepperX.runToNewPosition(0);
  stepperY.runToNewPosition(0);
}

void esp32Decoder(String data, bool* harvest, bool* plant, bool* status, int* id, int* tx_rx_id){
  int commaIndex, secondCommaIndex = 0;
  commaIndex = data.indexOf(',');
  secondCommaIndex = data.indexOf(',', commaIndex + 1);
  String comm_id = data.substring(0, commaIndex); // parse first word and remove whitespace
  if(verbose) Serial.print("\nComm Id: ");
  if(verbose) Serial.println(comm_id);
  *tx_rx_id = comm_id.toInt();
  if(verbose) Serial.println(*tx_rx_id);
  String number = data.substring(commaIndex+1, secondCommaIndex); // parse plant number
  number.trim();
  *id = number.toInt();
  String operation = data.substring(secondCommaIndex + 1); // parse last word and remove whitespace
  operation.trim();

  if(operation == "Harvest" && *status == true){
    *harvest = true;
    *status = false;
  } else if(operation == "plant" && *status == true){
    *plant = true;
    *status = false;
  }

}

int plantIDtoXposition(int id){
  if(id == 1){
    return plant_1_x;
  } else if(id == 2){
    return plant_2_x;
  } else if(id == 3){
    return plant_3_x;
  } else if(id == 4){
    return plant_4_x;
  } else if(id == 5){
    return plant_5_x;
  } else if(id == 6){
    return plant_6_x;
  } else {
    //error
    Serial.print("ERROR, not possible ID value");
    reset();
    exit(0);
  }
}

int plantIDtoYposition(int id){
  if(id == 1){
    return plant_1_y;
  } else if(id == 2){
    return plant_2_y;
  } else if(id == 3){
    return plant_3_y;
  } else if(id == 4){
    return plant_4_y;
  } else if(id == 5){
    return plant_5_y;
  } else if(id == 6){
    return plant_6_y;
  } else {
    //error
    Serial.print("ERROR, not possible ID value");
    reset();
    exit(0);
  }
}

void printDebug(char* message, int value){
  Serial.print(message); // for debug purposes
  Serial.print(value);
  Serial.print("\n");
}