#include <DRV8835MotorShield.h>

#include <PS2X_lib.h>
#include <Servo.h>


PS2X ps2x;                             // create ps2 controler object
Servo servo;
DRV8835MotorShield motor;

//ps2 control pins
const int ps_clock = 23;       //blue     //12        
const int ps_command = 3;      //white   //11
const int ps_attention = 2;    //green   //10
const int ps_data = 22;        // gray    //13
boolean ps_pressSens = false;
boolean ps_rumble = false;

// servo and led pins
const int servoPin = 5;
const int redLed = 13;
const int yellowLed = 12;

// declare variables
int steer;
int velocity;

//initializing steering degrees
int straight = 80;
int rightMAX = 50;
int leftMAX = 110;

//ps2 control steering 
int analogMIN = 0;
int analogMAX = 255;
int middle = 0.5 * analogMAX;
int rightThresh = middle + 0.1*analogMAX;  //152
int leftThresh = middle - 0.1*analogMAX;   //101

int upThresh = middle - 0.1*analogMAX;     //101
int downThresh = middle + 0.1*analogMAX;   //152
//initialize DC motors
#define maxSpeed 400

int maxDesiredSpeed = 0.55*maxSpeed;
int minSpeed = 0;


void setup() {
  Serial.begin(57600);
  
  servo.attach(servoPin);
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, LOW);
  
  // config_game_pad(clock, command, attention, data, pressure sensitivity enabled, rumble enabled);
  ps2x.config_gamepad(ps_clock, ps_command, ps_attention, ps_data, ps_pressSens, ps_rumble);   

  Serial.println("\nSONY PlayStation 2");
  delay(2000);
  Serial.println("Dual Shock 2 controller connected.");

}

void loop() {
  ps2x.read_gamepad();

  
  if(ps2x.ButtonPressed(PSB_START)) {
    Serial.println("Start");
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, HIGH);
  }
  
  if(ps2x.ButtonPressed(PSB_SELECT)) {
    Serial.println("Select");
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
  }

  if(ps2x.ButtonPressed(PSB_PAD_LEFT)) {
    Serial.println("Left");
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, HIGH);
  }

  if(ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
    Serial.println("Right");
    digitalWrite(yellowLed, HIGH);
    digitalWrite(redLed, LOW);
  }

  if(ps2x.Analog(PSS_RX) > rightThresh) {
    steer = map(ps2x.Analog(PSS_RX), rightThresh, analogMAX, straight, rightMAX); 
    servo.write(steer);
  }
  else if(ps2x.Analog(PSS_RX < leftThresh)) {
    steer = map(ps2x.Analog(PSS_RX), leftThresh, analogMIN, straight, leftMAX);  
    servo.write(steer);
  }
  else {
    servo.write(straight);
  }

  if(ps2x.Button(PSB_R2)) {
    motor.setM1Speed(minSpeed);
    motor.setM2Speed(minSpeed);
  }
  else if(ps2x.Analog(PSS_LY) < upThresh) {
    velocity = map(ps2x.Analog(PSS_LY), upThresh, analogMIN, minSpeed, maxDesiredSpeed);  //101
    motor.setM1Speed(velocity);
    motor.setM2Speed(velocity);
  }
  else if(ps2x.Analog(PSS_LY) > downThresh) {
    velocity = map(ps2x.Analog(PSS_LY), downThresh, analogMAX, minSpeed, -maxDesiredSpeed); //152
    motor.setM1Speed(velocity);
    motor.setM2Speed(velocity);
  }
  else {
    motor.setM1Speed(minSpeed);  //0
    motor.setM2Speed(minSpeed);
  }
  
  
}




  
