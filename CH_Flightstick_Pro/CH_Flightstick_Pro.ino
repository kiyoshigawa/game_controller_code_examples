/*
CH Pro Joystick hijacking program.
Created by: kiyoshigawa
date: 2019-03-11
This is a simple arcade controller input box built for a teensy LC. 
It uses 8 IO pins, and outputs them as joystick button presses.
It also uses 3 analog inputs and outputs them as joystick axes.
*/


//Uncomment KEYPRESS_DEBUG to get feedback on key presses and releases sent
#define KEYPRESS_DEBUG

#include <Bounce.h>

//Number of buttons
#define NUM_BUTTONS 8

//Number of analog inputs
#define NUM_ANALOG 3

//set this to a number of milliseconds to wait for a serial connection before continuing with normal setep.
#define SERIAL_WAIT_TIMEOUT 5000

//number of ms to ignore changes in button state after a change
#define BOUNCE_TIME 5

//number of analog readings to average
#define NUM_ANALOG_READS_TO_AVERAGE 20

//Default value for analog axes:
#define DEFAULT_JOYSTICK_VALUE 512

//global variables, because I can.

//array of button pins
int buttons[] = {0, 1, 2, 3, 4, 5, 6, 7};

//array of analog pins
int analog_inputs[] = {A0, A1, A2};

//index for averaging
int averaging_index = 0;

//Arrays for storing readings to be averaged
//The first NUM_ANALOG_READS_TO_AVERAGE is for the first axis, the next NUM_ANALOG_READS_TO_AVERAGE is for the second, etc. 
//The array is big enough to hold NUM_ANALOG inputs for averaging.
int analog_reading_index[NUM_ANALOG_READS_TO_AVERAGE*NUM_ANALOG] = {};

//init bounce objects for all buttons;
Bounce b1 = Bounce(buttons[0], BOUNCE_TIME);
Bounce b2 = Bounce(buttons[1], BOUNCE_TIME);
Bounce b3 = Bounce(buttons[2], BOUNCE_TIME);
Bounce b4 = Bounce(buttons[3], BOUNCE_TIME);
Bounce b5 = Bounce(buttons[4], BOUNCE_TIME);
Bounce b6 = Bounce(buttons[5], BOUNCE_TIME);
Bounce b7 = Bounce(buttons[6], BOUNCE_TIME);
Bounce b8 = Bounce(buttons[7], BOUNCE_TIME);

//array of bounce objects to iterate through
Bounce button_array[] = {b1, b2, b3, b4, b5, b6, b7, b8};

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
  for(int i=0; i<NUM_BUTTONS; i++){
    pinMode(buttons[i], INPUT_PULLUP);
    #ifdef KEYPRESS_DEBUG
      Serial.print("Set up P1 ");
      Serial.println(buttons[i]);
    #endif
  }
  
  //set analog pin as an input
  for(int i=0; i<NUM_ANALOG; i++){
    pinMode(analog_inputs[i], INPUT);
    #ifdef KEYPRESS_DEBUG
      Serial.print("Set up Analog");
      Serial.println(buttons[i]);
    #endif
  }

  //Default to center for joystick axes
  Joystick.X(DEFAULT_JOYSTICK_VALUE);
  Joystick.Y(DEFAULT_JOYSTICK_VALUE);
  Joystick.Z(DEFAULT_JOYSTICK_VALUE);
  Joystick.Zrotate(DEFAULT_JOYSTICK_VALUE);
  Joystick.sliderLeft(DEFAULT_JOYSTICK_VALUE);
  Joystick.sliderRight(DEFAULT_JOYSTICK_VALUE);

  //fill averaging arrays with default values
  for(int i=0; i<NUM_ANALOG_READS_TO_AVERAGE*NUM_ANALOG; i++){
    analog_reading_index[i] = DEFAULT_JOYSTICK_VALUE;
  }

}

void loop(){
  update_buttons();
  update_analog();
  set_joystick_axes();
}

//check state of buttons for change (bounced) and update joystick status based on this.
void update_buttons(){
  for(int i=0; i<NUM_BUTTONS; i++){
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

//This reads all the analog axes and sends the output directly to the joystick axes.
void update_analog(){
  for(int i=0; i<NUM_ANALOG; i++){
    //read each pin and store it temporarily.
    int current_analog_reading = analogRead(analog_inputs[i]);
    //This will save thge current variable to the right indexed part of the analog_reading_index array:
    analog_reading_index[i*NUM_ANALOG_READS_TO_AVERAGE+averaging_index] = current_analog_reading;
  }

  //increment the averaging index each reading:
  averaging_index++;
  averaging_index %= NUM_ANALOG_READS_TO_AVERAGE;
}

void set_joystick_axes(){
  //Doing all this manually, because it is easier to understand. (Thanks, Parker)

  //Average each axis and set the joystick axis correctly.
  int x_average = 0;
  int y_average = 0;
  int z_average = 0;
  for(int i=0; i<NUM_ANALOG_READS_TO_AVERAGE; i++){
    //Manually jumping ahead in the array to make the code easier to follow, instead of using a nested for loop.
    x_average = x_average + analog_reading_index[NUM_ANALOG_READS_TO_AVERAGE*0+i];
    y_average = y_average + analog_reading_index[NUM_ANALOG_READS_TO_AVERAGE*1+i];
    z_average = z_average + analog_reading_index[NUM_ANALOG_READS_TO_AVERAGE*2+i];
  }
  
  //divide by the number of samples:
  x_average = x_average/NUM_ANALOG_READS_TO_AVERAGE;
  y_average = y_average/NUM_ANALOG_READS_TO_AVERAGE;
  z_average = z_average/NUM_ANALOG_READS_TO_AVERAGE;

  //finally send the average to the joystick object:
  //wired it backwards
  Joystick.X(1024-x_average);
  Joystick.Y(y_average);
  Joystick.Z(z_average);
}
