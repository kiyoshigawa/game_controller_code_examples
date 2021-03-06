/*
This is a simple program to demonstrate how to set up multiple software-bounced button inputs.
This was prepared for a class by kiyoshigawa on 2019-03-21 regarding how to make video game controllers using teensy.
*/

//include libraries
#include "Bounce.h"

//comment to disable USB serial debug messages
#define DEBUG


//define constants and pin numbers:

//set this to a number of milliseconds to wait for a serial connection before continuing with normal setep.
#define SERIAL_WAIT_TIMEOUT 10000

//number of ms to ignore changes in button state after a change
#define BOUNCE_TIME 5

//Number of buttons
#define NUM_BUTTONS 3

//array of button pins
int button_pins[NUM_BUTTONS] = {2, 3, 4};


//initialize a bounce object for the pin:
Bounce b1 = Bounce(button_pins[0], BOUNCE_TIME);
Bounce b2 = Bounce(button_pins[1], BOUNCE_TIME);
Bounce b3 = Bounce(button_pins[2], BOUNCE_TIME);

//make an array with all the buttons to iterate through
Bounce button_array[NUM_BUTTONS] = {b1, b2, b3};

//Setup function - runs once at the start:

void setup() {
  //Serial debug output setup:
  Serial.begin(9600);
  while(!Serial){
    if(millis() > SERIAL_WAIT_TIMEOUT){
      break;
    }
  }
  
  #ifdef DEBUG
    Serial.println("Keypress Testing:");
  #endif
  
  //init pins as inputs with pullup resistors
  for(int i=0; i<NUM_BUTTONS; i++){
    pinMode(button_pins[i], INPUT_PULLUP);
    #ifdef DEBUG
      Serial.print("Set up buttons ");
      Serial.println(button_pins[i]);
    #endif
  }

  //disable other joystick functions - these will be sending 0 constantly if they aren't set to the middle
  Joystick.X(512);
  Joystick.Y(512);
  Joystick.Z(512);
  Joystick.Zrotate(512);
  Joystick.sliderLeft(512);
  Joystick.sliderRight(512);

}

//Loop function - runs continuously:

void loop(){
  //this updates the button and will send a command if you want it to.
  update_buttons();

  //if you have lighting functions or something else, run them here:

}

//this function will check to see if the button state has changed, and can then do an action if you want:

void update_buttons(){
  for(int i=0; i<NUM_BUTTONS; i++){
    //check for updates on each button
    if(button_array[i].update()){
      //if the button was released set it to 0
      if(button_array[i].risingEdge()){
        //Send the joystick released command for the right button.
        Joystick.button(i+1, 0); //+1 because joystick buttons are from 1-12, not 0-11.
        #ifdef DEBUG
          Serial.print("Released ");
          Serial.println(i+1);
        #endif
      }
      //otherwise set it to pressed
      else if(button_array[i].fallingEdge()){
        //Send the joystick pressed command for the right button.
        Joystick.button(i+1, 1); //+1 because joystick buttons are from 1-12, not 0-11.
        #ifdef DEBUG
          Serial.print("Pressed ");
          Serial.println(i+1);
        #endif
      }
    }
  }
}
