/*
This is a simple program to demonstrate how to set up a simple analog input that takes averaged readings and outputs them as a joystick axis.
This was prepared for a class by kiyoshigawa on 2019-03-21 regarding how to make video game controllers using teensy.
*/

//set this to a number of milliseconds to wait for a serial connection before continuing with normal setep.
#define SERIAL_WAIT_TIMEOUT 10000

//number of analog readings to average
#define NUM_ANALOG_READS_TO_AVERAGE 20

//Default value for analog axes:
#define DEFAULT_JOYSTICK_VALUE 512

//analog input pin number
int analog_input_pin = A0;

//index for averaging - this will increment through the array of values being averaged.
int averaging_index = 0;

//Arrays for storing readings to be averaged
int analog_reading_index[NUM_ANALOG_READS_TO_AVERAGE] = {};


void setup() {
  Serial.begin(9600);
  while(!Serial){
    if(millis() > SERIAL_WAIT_TIMEOUT){
      break;
    }
  }

  //set analog pin as an input
  pinMode(analog_input_pin, INPUT);

  //Default to center for joystick axes
  Joystick.X(DEFAULT_JOYSTICK_VALUE);
  Joystick.Y(DEFAULT_JOYSTICK_VALUE);
  Joystick.Z(DEFAULT_JOYSTICK_VALUE);
  Joystick.Zrotate(DEFAULT_JOYSTICK_VALUE);
  Joystick.sliderLeft(DEFAULT_JOYSTICK_VALUE);
  Joystick.sliderRight(DEFAULT_JOYSTICK_VALUE);

  //fill averaging arrays with default values
  for(int i=0; i<NUM_ANALOG_READS_TO_AVERAGE; i++){
    analog_reading_index[i] = DEFAULT_JOYSTICK_VALUE;
  }

}

void loop(){
  update_analog();
  set_joystick_axes();
}

//This reads all the analog axes and sends the output directly to the joystick axes.
void update_analog(){
  //read each pin and store it temporarily.
  int current_analog_reading = analogRead(analog_input_pin);
  //This will save thge current variable to the right indexed part of the analog_reading_index array:
  analog_reading_index[averaging_index] = current_analog_reading;

  //increment the averaging index each reading:
  averaging_index++;
  averaging_index %= NUM_ANALOG_READS_TO_AVERAGE;
}

void set_joystick_axes(){
  //Average all the readings currently in the reading array
  int average = 0;
  for(int i=0; i<NUM_ANALOG_READS_TO_AVERAGE; i++){
    //Manually jumping ahead in the array to make the code easier to follow, instead of using a nested for loop.
    average = average + analog_reading_index[i];
  }
  
  //divide by the number of samples:
  average = average/NUM_ANALOG_READS_TO_AVERAGE;

  //finally send the average to the joystick object:
  //wired it backwards
  Joystick.X(average);
}
