/*
This is a simple program to demonstrate how to set up a quadrature encoder for use as a game controller input.
This was prepared for a class by kiyoshigawa on 2019-03-21 regarding how to make video game controllers using teensy.
*/

//Uncomment ENCODER_DEBUG to get serial feedback on encoder position.
#define ENCODER_DEBUG

#include <Encoder.h>

//defines to make the code more readable:
#define POSITIVE 1
#define STOPPED 0
#define NEGATIVE -1
#define LEFT 0
#define RIGHT 1

//this is the number of miliseconds from the last movement that will register as still pressing a key for scrolling purposes. If the wheel does not move for MOVE_TIMEOUT ms, it will stop pressing up or down. If the wheel changes direction before MOVE_TIMEOUT, it will still send the alternate direction as soon as the change is detected.
#define MOVE_TIMEOUT 100

//set this to a number of milliseconds to wait for a serial connection before continuing with normal setep.
#define SERIAL_WAIT_TIMEOUT 10000

//this is the minimum number of steps before the motion of the encoders registers. Decrease to make the wheel more sensitive, increase to make the wheel less sensitive. If this number is too low, the wheel may register hits when it is not being moved.
#define STEP_THRESHOLD 20

//if encoders are running backwards, swap pin a and b.
int encoder_pin_a = 5;
int encoder_pin_b = 6;

//init encoder objects
Encoder knob(encoder_pin_a, encoder_pin_b);

//global variables used below
long encoder_position = 0;
int encoder_direction = STOPPED;
unsigned long last_encoder_move_time = 0;
bool encoder_has_stopped = true;
//variables to keep track of direction of travel
int last_encoder_direction = POSITIVE;

void setup(){
  Serial.begin(9600);
  while(!Serial){
    if(millis() > SERIAL_WAIT_TIMEOUT){
      break;
    }
  }
  #ifdef ENCODER_DEBUG
    Serial.println("Encoder Test:");
  #endif

  //disable other joystick functions
  Joystick.X(512);
  Joystick.Y(512);
  Joystick.Z(512);
  Joystick.Zrotate(512);
  Joystick.sliderLeft(512);
  Joystick.sliderRight(512);

}
  
void loop(){
    update_encoders();
    encoder_key_press();
}

//this will update the time variables showing when the encoders last changed position.
void update_encoders(){
  long new_pos;
  unsigned long current_time = millis();
  new_pos = knob.read();
  
  //if going positive direction
  if (new_pos > (encoder_position+STEP_THRESHOLD) ) {
    encoder_position = new_pos;
	  encoder_direction = POSITIVE;
	  last_encoder_move_time = current_time;
    encoder_has_stopped = false;
    print_encoder_position();
  }
  //if going negative direction
  else if (new_pos < (encoder_position-STEP_THRESHOLD) ) {
    encoder_position = new_pos;
    encoder_direction = NEGATIVE;
    last_encoder_move_time = current_time;
    encoder_has_stopped = false;
    print_encoder_position();
  }
  //if it has not changed position since MOVE_TIMEOUT, stop sending either up or down
  else if ((current_time - last_encoder_move_time) > MOVE_TIMEOUT){
    encoder_direction = STOPPED;
  }
  
  // if the value approaches the max, gracefully reset values to 0.
  if (abs(encoder_position) > 2000000000) {
    knob.write(0);
    encoder_position = 0;
  }
}

//this will use the timing from the update_encoders() function to determine if the joystick should be pressed up, down, or not at all. Note that the joystick mapped buttons for up and down will be NUM_BUTTONS + 1, 2, 3, or 4, as the first NUM_BUTTONS buttons will be used by the keys, start, and select buttons.
void encoder_key_press(){
  if(encoder_direction == POSITIVE){
    //press up button and release down button
    Joystick.button(1, 1);
    Joystick.button(2, 0);
  }
  else if(encoder_direction == NEGATIVE){
    //press down button and release up button
    Joystick.button(1, 0);
    Joystick.button(2, 1);
  }
  else if(encoder_direction == STOPPED){
    //release both up and down buttons
    Joystick.button(1, 0);
    Joystick.button(2, 0);
    if(encoder_has_stopped == false){
      encoder_position = knob.read();
      #ifdef ENCODER_DEBUG
        Serial.print("Stopped at: ");
      #endif
      print_encoder_position();
      encoder_has_stopped = true;
    }
  }
}

void print_encoder_position(){
	#ifdef ENCODER_DEBUG  
    Serial.print("Position = ");
    Serial.print(encoder_position);
    Serial.println();
  #endif
}
