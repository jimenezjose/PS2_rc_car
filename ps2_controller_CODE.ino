/*
 * Jose Jimenez   
 * 
 * (Arduino Mega 2560)
 * The purpose of this program is to be able to use a Playstation 2 controller to send  
 * intructions to the remote control car. The left and right joysticks were used as analog 
 * inputs for the mapped output of the rotational speed of the two DC motors, and the 
 * direction of the servo(and car) in degrees, respectively. The left and right pad buttons 
 * were digital inputs for the leds (signal lights). Lastly, the R2 button was used as a digital 
 * input for completely stopping the rotation of the DC motors, acting as a brake to the car.
 * 
 */

#include <DRV8835MotorShield.h>                         // load library for the DC moltor bridge  //https://github.com/pololu/drv8835-motor-shield 
#include <PS2X_lib.h>                                   // load library for special features of the Playstation 2 controller  //https://github.com/madsci1016/Arduino-PS2X
#include <Servo.h>                                      // load Servo library  


PS2X ps2x;                                              // create PS2X object in variable ps2x
Servo servo;                                            //create Servo object in the variable named servo
DRV8835MotorShield motor;                               // create DRV8835MotorShield in the variable namesd motor

//ps2 control pins
const int ps_clock = 23;       //blue     //12          //  ps_clock is attached to pin 23   
const int ps_command = 3;      //white    //11          // ps_command is attached to pin 3
const int ps_attention = 2;    //green    //10          // ps_attention is attached to pin 2
const int ps_data = 22;        // gray    //13          // ps_data is attached to pin 22
boolean ps_pressSens = false;                           // the pressure sensitivity is disabled, that's why it is false
boolean ps_rumble = false;                              // the rumble from the Playstation controller is disabled

// servo and led pins
const int servoPin = 5;                                 // the servo is connected to pin 5
const int redLed = 13;                                  // the redLed is connected to pin 13
const int yellowLed = 12;                               // the yellowLed is attached to pin 12

// declare variables
int steer;                                              // declare steer as type int.  
int velocity;                                           // declare velocity as type int

//initializing steering degrees
int straight = 80;                                      // straight will be 80 degrees on the servo.
int rightMAX = 50;                                      // the most right that the servo will go is 50 degrees
int leftMAX = 110;                                      // the most left that the servo will go is 110 degrees

//ps2 control steering 
int analogMIN = 0;                                      // the joystick on the ps2 controller reads from (0,250) therefore min is analogMIN is 0
int analogMAX = 255;                                    // analogMAX is initialized to 255
int middle = 0.5 * analogMAX;                           // the middle of 0-250 is 250/2 
int rightThresh = middle + 0.1*analogMAX;  //152        // (right joystick) the rightThreshold is the middle + 10% of 250  // 10% is used for room for error
int leftThresh = middle - 0.1*analogMAX;   //101        // (right joystick) the left threshold is middle - 10% of 250

/* 0 is the output when the left 
 * joystick is all the way up 
 */
int upThresh = middle - 0.1*analogMAX;     //101        // (left joystick) the up threshold is middle - 10% of 250 
int downThresh = middle + 0.1*analogMAX;   //152        // (left jpystick) the down threshold is middle - 10% of 250
//initialize DC motors
#define maxSpeed 400                                    // max speed for DC motor is 400, not sure what the units are

int maxDesiredSpeed = 0.55*maxSpeed;                    // max desired speed is 55% of 400 (Now that I think about it I think maxspeed should be set to 255 that way you won't need this variable)
int minSpeed = 0;                                       // minSpeed is initialized to 0

/*
 * the setup() function is used to initialize
 * what is needed for the upcomming loop to 
 * execute successfully. setup() is only executed 
 * once throughout the program.
 */
void setup() {
  Serial.begin(57600);                                  // MAKE sure baud rate is 57600 bits per second so that the communication between the control and 
                                                        // the ardiuno is fluent. (If on Serial Monitor make sure of this rate as well)
  servo.attach(servoPin);                               // servo is attached to servoPin
  pinMode(redLed, OUTPUT);                              // initialize redLed as an output
  pinMode(yellowLed, OUTPUT);                           // initialize yellowLed as an output
  
  digitalWrite(redLed, LOW);                            // although these next two steps are unnecessary I initialized the Leds to be off or LOW
  digitalWrite(yellowLed, LOW);
  
  // config_game_pad(clock, command, attention, data, pressure sensitivity enabled, rumble enabled);
  ps2x.config_gamepad(ps_clock, ps_command, ps_attention, ps_data, ps_pressSens, ps_rumble);   

  Serial.println("\nSONY PlayStation 2");                // In the serial monitor print out the String argument.
  delay(2000);                                           // wait for 2000 ms (2 seconds)
  Serial.println("Dual Shock 2 controller connected.");  // print out String argument

}

/*
 * loop() is the main function that creates the magic
 * and logic to the program. The loop() function is 
 * executed repeatedly and the purpose of this function
 * is to give the ability to send commands to the remote
 * controlled car using a Playstation controller.
 */
void loop() {
  ps2x.read_gamepad();                                   // read the input sent from the ps2 controller

  
  if(ps2x.ButtonPressed(PSB_START)) {                    // if the start button is pressed, turn on the red and yellow led.
    Serial.println("Start");                             // print statement for user friendliness.
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, HIGH);
  }
  
  if(ps2x.ButtonPressed(PSB_SELECT)) {                   // if the select button is true then, turn off red and yellow led.
    Serial.println("Select");                            // print statement for user friendliness.
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
  }

  if(ps2x.ButtonPressed(PSB_PAD_LEFT)) {                 // if the left pad button is pressed turn on the red led and turn off the yellow led
    Serial.println("Left Signal Light");
    digitalWrite(yellowLed, LOW);
    digitalWrite(redLed, HIGH);
  }

  if(ps2x.ButtonPressed(PSB_PAD_RIGHT)) {                // if the right pad butto is pressed turn on yellow led and off the red led
    Serial.println("Right Signal Light");
    digitalWrite(yellowLed, HIGH);
    digitalWrite(redLed, LOW);
  }
/* 
 * The right joystick outputs a value of 255(max) 
 * when all the way to the right and o when joystick  
 * is all the way to the left.
 */
  if(ps2x.Analog(PSS_RX) > rightThresh) {                // if Right joystick is moved in the x direction and is greater than rightThresh execute the following  
    steer = map(ps2x.Analog(PSS_RX), rightThresh, analogMAX, straight, rightMAX);   // map what is read from the joy stick (rightThresh, MAX) to the degrees of servo from (straight, rightMAX)
    servo.write(steer);                                  // rotate the servo based the degree output of the variable steer
  }
  else if(ps2x.Analog(PSS_RX < leftThresh)) {            // else if the joystick reads less than the leftThresh then execute
    steer = map(ps2x.Analog(PSS_RX), leftThresh, analogMIN, straight, leftMAX);     // assign steer, to what is read from the joystck (leftThresh, analogMIN) and convert to (straight, leftMAX) degrees
    servo.write(steer);                                  // rotate servo based on the output of steer
  }
  else {
    servo.write(straight);                               //otherwise (10% slack on both sides (right and left)) leave the servo to point straight
  }

  if(ps2x.Button(PSB_R2)) {                              // if R2 is pressed then brake! (it is important that this is the beginning of the if-else statement)
    motor.setM1Speed(minSpeed);                          // set the 1st and 2nd DC motor to 0
    motor.setM2Speed(minSpeed);
  }
/*
 * remember this is on the left joystick and we are reading
 * the output from the vertical direction. When the joystick 
 * is all the way UP, the output is read as 0 and when the 
 * joystick is all the way DOWN what is read is 255.
 */
  else if(ps2x.Analog(PSS_LY) < upThresh) {              // if the joystick is less the upThresh execute the following if statement
    velocity = map(ps2x.Analog(PSS_LY), upThresh, analogMIN, minSpeed, maxDesiredSpeed);  //101 // assign the conversion to velocity. from (upThresh to analogMIN) convert to (minSpeed to maxDesiredSpeed)
    motor.setM1Speed(velocity);                          // set motor speed to velocity
    motor.setM2Speed(velocity);
  }
  else if(ps2x.Analog(PSS_LY) > downThresh) {            // if the joystick is read to be greater than the downThresh execute the following
    velocity = map(ps2x.Analog(PSS_LY), downThresh, analogMAX, minSpeed, -maxDesiredSpeed); //152 // read the joystick input from (downThresh to analogMAX) and convert that to (minSpeed to -maxDesiredSpeed) (negative max)
    motor.setM1Speed(velocity);                          // set speed to the mapped output, velocity.
    motor.setM2Speed(velocity);
  }
  else {
    motor.setM1Speed(minSpeed);  //0                     // otherwise set the speed to 0
    motor.setM2Speed(minSpeed);
  }
  
  
}




  
