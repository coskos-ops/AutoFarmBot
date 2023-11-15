// Joystick pin configurations
const int X_ctrl_pin = A0; // analog pin - values from 0 to 1024
const int Y_ctrl_pin = A1; // analog pin - values from 0 to 1024
const int Z_ctrl_pin = A2; // analog pin - values from 0 to 1024

// variables to store direction
int x_dir = 0; // 0 = no movement, 1 = CW rotation, 2 = CCW rotation
int y_dir = 0; // 0 = no movement, 1 = CW rotation, 2 = CCW rotation
int z_dir = 0; // 0 = no movement, 1 = CW rotation, 2 = CCW rotation

// Motor pin configurations
const int X_step_pin = 2;
const int X_dir_pin = 5;
const int Y_step_pin = 3;
const int Y_dir_pin = 6;
const int Z_step_pin = 4;
const int Z_dir_pin = 7;

// User define speed for all motors
const int motorDelay = 80;

// debug mode
bool verbose = false;

void setup() {
  // code run once to declare pin types
  pinMode(X_ctrl_pin, INPUT);
  pinMode(Y_ctrl_pin, INPUT);
  pinMode(Z_ctrl_pin, INPUT);

  pinMode(X_step_pin, OUTPUT);
  pinMode(X_dir_pin, OUTPUT);
  pinMode(Y_step_pin, OUTPUT);
  pinMode(Y_dir_pin, OUTPUT);
  pinMode(Z_step_pin, OUTPUT);
  pinMode(Z_dir_pin, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT); // led to indicate active operation

  if(verbose) Serial.begin(115200); // for debug purposes
  //if(verbose) digitalWrite(X_dir_pin, HIGH);
  //if(verbose) digitalWrite(X_step_pin, HIGH);
}

void loop() {
  // read joystick
  if(verbose) Serial.print("Loop: \n");
  if(verbose) debugFunction();
  translateJoystickToDirections();
  directionsToMotorOutputs(x_dir, X_step_pin, X_dir_pin);
  directionsToMotorOutputs(y_dir, Y_step_pin, Y_dir_pin);
  directionsToMotorOutputs(z_dir, Z_step_pin, Z_dir_pin); // Not sure how z axis is configured on Joystick
  if(verbose) delay(1000); // one second delay
}

void directionsToMotorOutputs(int direction, int axis_step_pin, int axis_dir_pin){
  if(verbose) Serial.print("Motor Activation\n"); // for debug purposes
  if(direction == 0){ // no movement based on Joystick
    digitalWrite(axis_step_pin, LOW);
    delayMicroseconds(motorDelay);
  } else if (direction == 1) { // positive direction based on Joystick
    digitalWrite(axis_dir_pin, HIGH);
    digitalWrite(axis_step_pin, HIGH);
    delayMicroseconds(motorDelay);
    digitalWrite(axis_step_pin, LOW);
    delayMicroseconds(motorDelay);
  } else if (direction == 2){ // negative direction based on Joystick
    digitalWrite(axis_dir_pin, LOW);
    digitalWrite(axis_step_pin, HIGH);
    delayMicroseconds(motorDelay);
    digitalWrite(axis_step_pin, LOW);
    delayMicroseconds(motorDelay);
  }
}

void translateJoystickToDirections(){
  x_dir = translationHelper(analogRead(X_ctrl_pin), y_dir, z_dir);
  y_dir = translationHelper(analogRead(Y_ctrl_pin), x_dir, z_dir);
  z_dir = translationHelper(analogRead(Z_ctrl_pin), x_dir, y_dir);
}

int translationHelper(int pin, int otherPin1, int otherPin2){
  // only control one motor at a time
  if (pin < 400 && otherPin1 == 0 && otherPin2 == 0) { // Joystick control left/up
    return 1;
  } else if (pin > 600 && otherPin1 == 0 && otherPin2 == 0) { // Joystick control right/down
    return 2;
  } else {
    return 0;
  }
}

void debugFunction(){
  printDebug("X-axis: ", analogRead(X_ctrl_pin));
  printDebug("Y-axis: ", analogRead(Y_ctrl_pin));
  printDebug("Z-axis: ", analogRead(Z_ctrl_pin));
  printDebug("X Direction: ", x_dir);
  printDebug("Y Direction: ", y_dir);
  printDebug("Z Direction: ", z_dir);
}

void printDebug(char* message, int value){
  Serial.print(message); // for debug purposes
  Serial.print(value);
  Serial.print("\n");
}
