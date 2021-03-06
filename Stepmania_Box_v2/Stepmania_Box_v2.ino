/*
Stepmania Controller Box v2 - 2P simple controller
Created by: kiyoshigawa
date: 2018-08-18
This is a simple arcade controller input box built for a teensy LC. 
It uses 12 IO pins, and outputs them as joystick button presses.
*/


//Uncomment KEYPRESS_DEBUG to get feedback on key presses and releases sent
#define KEYPRESS_DEBUG

#include <Bounce.h>

//Number of buttons per player
#define NUM_BUTTONS_PER_PLAYER 6

//set this to a number of milliseconds to wait for a serial connection before continuing with normal setep.
#define SERIAL_WAIT_TIMEOUT 10000

//number of ms to ignore changes in button state after a change
#define BOUNCE_TIME 5

//array of button pins
int p1_buttons[] = {0, 1, 2, 3, 4, 5};
int p2_buttons[] = {6, 7, 8, 9, 10, 11};

//init bounce objects for all buttons;
Bounce p1_b1 = Bounce(p1_buttons[0], BOUNCE_TIME);
Bounce p1_b2 = Bounce(p1_buttons[1], BOUNCE_TIME);
Bounce p1_b3 = Bounce(p1_buttons[2], BOUNCE_TIME);
Bounce p1_b4 = Bounce(p1_buttons[3], BOUNCE_TIME);
Bounce p1_b5 = Bounce(p1_buttons[4], BOUNCE_TIME);
Bounce p1_b6 = Bounce(p1_buttons[5], BOUNCE_TIME);

Bounce p2_b1 = Bounce(p2_buttons[0], BOUNCE_TIME);
Bounce p2_b2 = Bounce(p2_buttons[1], BOUNCE_TIME);
Bounce p2_b3 = Bounce(p2_buttons[2], BOUNCE_TIME);
Bounce p2_b4 = Bounce(p2_buttons[3], BOUNCE_TIME);
Bounce p2_b5 = Bounce(p2_buttons[4], BOUNCE_TIME);
Bounce p2_b6 = Bounce(p2_buttons[5], BOUNCE_TIME);

//array of bounce objects to iterate through
Bounce button_array[] = {p1_b1, p1_b2, p1_b3, p1_b4, p1_b5, p1_b6, p2_b1, p2_b2, p2_b3, p2_b4, p2_b5, p2_b6};

void setup() {
  Serial.begin(9600);
  while(!Serial){
    if(millis() > SERIAL_WAIT_TIMEOUT){
      break;
    }
  }
  
  #ifdef KEYPRESS_DEBUG
    Serial.println("Keypress Testing:");
  #endif
  
  //init pins as inputs with pullup resistors
  for(int i=0; i<NUM_BUTTONS_PER_PLAYER; i++){
    pinMode(p1_buttons[i], INPUT_PULLUP);
    #ifdef KEYPRESS_DEBUG
      Serial.print("Set up P1 ");
      Serial.println(p1_buttons[i]);
    #endif
    pinMode(p2_buttons[i], INPUT_PULLUP);
    #ifdef KEYPRESS_DEBUG
      Serial.print("Set up P2 ");
      Serial.println(p2_buttons[i]);
    #endif
  }

  //disable other joystick functions
  Joystick.X(512);
  Joystick.Y(512);
  Joystick.Z(512);
  Joystick.Zrotate(512);
  Joystick.sliderLeft(512);
  Joystick.sliderRight(512);

}

void loop(){
  update_buttons();
}

//check state of buttons for change (bounced) and update joystick status based on this.
void update_buttons(){
  for(int i=0; i<NUM_BUTTONS_PER_PLAYER*2; i++){
    //check for updates on each button
    if(button_array[i].update()){
      //if the button was released set it to 0
      if(button_array[i].risingEdge()){
        Joystick.button(i+1, 0); //+1 because joystick buttons are from 1-12, not 0-11.
        #ifdef KEYPRESS_DEBUG
          Serial.print("Released ");
          Serial.println(i+1);
        #endif
      }
      //otherwise set it to pressed
      else if(button_array[i].fallingEdge()){
        Joystick.button(i+1, 1); //+1 because joystick buttons are from 1-12, not 0-11.
        #ifdef KEYPRESS_DEBUG
          Serial.print("Pressed ");
          Serial.println(i+1);
        #endif
      }
    }
  }
}
