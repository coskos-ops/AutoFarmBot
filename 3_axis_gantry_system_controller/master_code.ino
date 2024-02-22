#include <AccelStepper.h>

// Reference Table for Stepper Distances of each plant with respect to Collection bin (position 0)
// Assumed ordering (motor is refering to XY direction - written twice but the same):
// Motor    Plant 1     Plant 3     Plant 5
// Motor    Plant 2     Plant 4     Plant 6

const int x1 = 600;
const int x2 = 1700;
const int x3 = 3000;
const int y1 = 600;
const int y2 = 1200;
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
const int plant_z = 1900; //location where cut occurs (2-3 cm from base)

// Motor pin configurations
const int X_step_pin = 2;
const int X_dir_pin = 5;
const int Y_step_pin = 3;
const int Y_dir_pin = 6;
const int Z_step_pin = 4;
const int Z_dir_pin = 7;

// User define steps per second for all motors
const int motorSpeed = 8000;
const int maxSpeed = 8500;
const int acceleration = 400;
int locations [6] = {2,3,5,1,4,6};

// debug mode
bool verbose = false;

// flags for communications
bool ready = true;
bool harvest = false; //signals from ESP
bool plant = false;

// for calculations
int plantID = 1; // values range from 1 to 6

AccelStepper stepperX(1, X_step_pin, X_dir_pin);
AccelStepper stepperY(1, Y_step_pin, Y_dir_pin);
AccelStepper stepperZ(1, Z_step_pin, Z_dir_pin);

void setup() {
  pinMode(X_step_pin, OUTPUT);
  pinMode(X_dir_pin, OUTPUT);
  pinMode(Y_step_pin, OUTPUT);
  pinMode(Y_dir_pin, OUTPUT);
  pinMode(Z_step_pin, OUTPUT);
  pinMode(Z_dir_pin, OUTPUT);

  Serial.begin(115200); // for communications 
  
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
}

void loop() {
  //Serial.print("Hi\n");
  if(verbose) Serial.print("Loop: \n");

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

  //if(Serial.available()){
    //String rxData = Serial.readStringUntil('\n');
    //String rxData = "plant,2\n";
    //if(verbose) Serial.print("ESP32 Says: " + rxData + "\n");
    //esp32Decoder(rxData, &harvest, &plant, &ready, &plantID);

    //test
  for (int i = 0; i < sizeof(locations); i++){
    harvest = true;
    plant = false;
    ready = false;
    plantID = locations[i];

  //}
  if(!ready){
    Serial.write("Busy\n"); // inform esp32 that no instruction will be accepted
  }
  if(verbose) printDebug("Harvest ", harvest);
  if(verbose) printDebug("Plant ", plant);
  if(verbose) printDebug("Ready ", ready);
  if(verbose) printDebug("Plant Number ", plantID);

  /*
      Based on comms, translate into position and move 3D gantry
  */
  
  if(harvest){
    if(verbose) Serial.print("Harvesting in progress \n");
    
    // go to plant
    stepperY.runToNewPosition(plantIDtoYposition(plantID));
    stepperX.runToNewPosition(plantIDtoXposition(plantID));
    stepperZ.runToNewPosition(plant_z);
    stepperZ.runToNewPosition(plant_z-500);

    // cut and grab plant (TBD KRIS) - signal to servo motor (sg90)

    // drop plant in container (TBD KRIS) and reset position
    //stepperZ.runToNewPosition(0);
    //stepperY.runToNewPosition(0);
    //stepperX.runToNewPosition(0);
    
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

    // plant seed (TBD KRIS)

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
    Serial.write("Ready\n"); // ready to fetch next instruction from esp32
  }
  
  if(verbose) delay(300); // 0.3 second delay
  }
  reset();
  if(verbose) Serial.print("Done\n");
  exit(0);
}

void reset (){
  stepperZ.runToNewPosition(0);
  stepperX.runToNewPosition(0);
  stepperY.runToNewPosition(0);
}

void esp32Decoder(String data, bool* harvest, bool* plant, bool* status, int* id){
  int commaIndex = 0;
  commaIndex = data.indexOf(',');
  String operation = data.substring(0, commaIndex); // parse first word and remove whitespace
  operation.trim();
  String number = data.substring(commaIndex+1); // parse plant number
  number.trim();
  *id = number.toInt();

  if(operation == "harvest" && *status == true){
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