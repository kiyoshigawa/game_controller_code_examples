/*
This is a simple program to demonstrate how to set up a single software-bounced button input.
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

//define the pin number:
int button_pin = 2;


//initialize a bounce object for the pin:
Bounce button = Bounce(button_pin, BOUNCE_TIME);


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
  
  //set pin mode for button:
  pinMode(button_pin, INPUT_PULLUP);

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
  //check for updates on each button
  if(button.update()){
    //if the button was released set it to 0
    if(button.risingEdge()){
      //the line below is for output when the button is released: 
      Joystick.button(1, 0);
      #ifdef DEBUG
        Serial.println("Released ");
      #endif
    }
    //otherwise set it to pressed
    else if(button.fallingEdge()){
      //the line below is for output when the button is pressed down:
      Joystick.button(1, 1);
      #ifdef DEBUG
        Serial.println("Pressed ");
      #endif
    }
  }
}
