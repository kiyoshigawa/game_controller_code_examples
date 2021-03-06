/*
Code for Phase Shift Guitar by Kiyoshigawa
Date: 2019-03-09
*/

//Includes:
#include "Bounce.h."
#include <Adafruit_NeoPixel.h>

//Debug Modes Toggle - Comment to disable debug serial messages
#define DEBUG

//Define Constants:
//Bounce Time in ms
#define BOUNCE_TIME 8

//Number of Buttons:
#define NUM_BUTTONS 10

//Number of WS2812 LEDs being controlled.
#define NUM_LEDS 15

//Serial Timeout if it doesn't connect before controller resumes normal operation without Serial.
#define SERIAL_WAIT_TIMEOUT 5000

//Controls the max brightness ot limit curent draw on the LED strip.
#define STRIP_BRIGHTNESS 150

//defines to make the code more readable:
#define POSITIVE 1
#define STOPPED 0
#define NEGATIVE -1
#define LEFT 0
#define RIGHT 1

//Lighting Defines:

//this is how often the lighting can update when being animated.
#define LIGHTING_REFRESH_DELAY 15

//this scaling factor allows for smoother transitions around the edges on slow_rotate style animations. The integer math rounding makes abrupt changes otherwise
#define LM_SLOW_ROTATE_SCALING_FACTOR 30000

//Variables for the number of frames per animation. The lighting runs at approximately 60fps with LIGHTING_REFRESH_DELAY set to 15.
#define LM_SLOW_ROTATE_FRAMES 150

//Pin Definitions:
int up_strum_pin = 0;
int down_strum_pin = 1;
int green_pin = 2;
int red_pin = 3;
int yellow_pin = 4;
int blue_pin = 5;
int orange_pin = 6;
int start_pin = 7;
int select_pin = 8;
int star_power_pin = 9;
int LED_pin = 17;

//this is an array of keypresses corresponding to the 10 buttons above in order.
int key_array[] = {MODIFIERKEY_RIGHT_SHIFT, KEY_ENTER, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_TAB, KEY_BACKSPACE, KEY_DELETE};

//create bounce objects for pin inputs:
Bounce up_strum_button = Bounce(up_strum_pin, BOUNCE_TIME);
Bounce down_strum_button = Bounce(down_strum_pin, BOUNCE_TIME);
Bounce green_button = Bounce(green_pin, BOUNCE_TIME);
Bounce red_button = Bounce(red_pin, BOUNCE_TIME);
Bounce yellow_button = Bounce(yellow_pin, BOUNCE_TIME);
Bounce blue_button = Bounce(blue_pin, BOUNCE_TIME);
Bounce orange_button = Bounce(orange_pin, BOUNCE_TIME);
Bounce start_button = Bounce(start_pin, BOUNCE_TIME);
Bounce select_button = Bounce(select_pin, BOUNCE_TIME);
Bounce star_power_button = Bounce(star_power_pin, BOUNCE_TIME);

//array for buttons for updating:
Bounce button_array[] = {up_strum_button, down_strum_button, green_button, red_button, yellow_button, blue_button, orange_button, start_button, select_button, star_power_button};

//NeoPixel Object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_pin, NEO_GRB + NEO_KHZ800);

//a color correction table for the neopixel color settings:
const uint8_t PROGMEM gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 
};

//Import Colors and Rainbows
#include "colors.h"

//initialize one rainbows array to hold all the rainbows:
rainbow rainbows[MAX_NUM_RAINBOWS] = {r1, r2, r3, r4, r5, r6, r7, r8, r9};

//random rainbow on startup
int current_rainbow = 0;

//Initialize the rainbow array to be the same size as the other arrays above. It will be set in the setup function and adjusted in the remaining program.
uint32_t rainbow[MAX_RAINBOW_COLORS];
int num_rainbow_colors = MAX_RAINBOW_COLORS;

//this is a variable for avoiding setting colors to be off based on the start of this table being mostly off. 
//It's still pretty rough near the low end with 8-bit colors. Not much I can do about it currently.
#define FIRST_NON_OFF_COLOR 28

//a new wrapper function to replace the strip.Color I have used previously. Still takes the same arguments and then calls the normal function with the table above as a reference.
void strip_setPixelColor(int led, uint32_t color){
  uint8_t red = (uint8_t)(color >> 16);
  uint8_t green = (uint8_t)(color >> 8);
  uint8_t blue = (uint8_t)(color);
  strip.setPixelColor(led, pgm_read_byte(&gamma8[red]), pgm_read_byte(&gamma8[green]), pgm_read_byte(&gamma8[blue]));
}

//Global Variables:
//This holds the current state of the buttons for use in lighting control
bool button_states[NUM_BUTTONS];

//This notes if a butotn has changed state - triggers a lighting event and should be reset once triggered to prevent events from going on too long.
bool button_has_triggered_lighting_event[NUM_BUTTONS];

//time variable for limiting lighting refresh rate:
unsigned long last_lighting_update = 0;

//int to keep track of state of fades and rotations
int lm_current_transition_position = 0;

//Setup Function for the Arduino:
void setup(){
  #ifdef DEBUG
    Serial.begin(9600);
    while(!Serial){
      if(millis() > SERIAL_WAIT_TIMEOUT){
        break;
      }
    }
    Serial.println("Serial Debug Active.");
  #endif

  //declare pin modes and set default state for outputs:
  pinMode(up_strum_pin, INPUT_PULLUP);
  pinMode(down_strum_pin, INPUT_PULLUP);
  pinMode(green_pin, INPUT_PULLUP);
  pinMode(red_pin, INPUT_PULLUP);
  pinMode(yellow_pin, INPUT_PULLUP);
  pinMode(blue_pin, INPUT_PULLUP);
  pinMode(orange_pin, INPUT_PULLUP);
  pinMode(start_pin, INPUT_PULLUP);
  pinMode(select_pin, INPUT_PULLUP);
  pinMode(star_power_pin, INPUT_PULLUP);
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, LOW);

  //Set Button States array to default:
  for(int i=0; i<NUM_BUTTONS; i++){
    button_states[i] = LOW;
    //Default all to false to force a default lighting event.
    button_has_triggered_lighting_event[i] = false;
  }

  //Initialize Lighting to Default State:
  strip.begin();
  strip.setBrightness(STRIP_BRIGHTNESS);
  lighting_init();

  //disable other joystick functions
  Joystick.X(512);
  Joystick.Y(512);
  Joystick.Z(512);
  Joystick.Zrotate(512);
  Joystick.sliderLeft(512);
  Joystick.sliderRight(512);
}

void loop(){
  //Update Buttons:
  update_buttons();

  //Run Lighting:
  lighting_control();

}

//check state of buttons for change (bounced) and update joystick status based on this.
void update_buttons(){
  for(int i=0; i<NUM_BUTTONS; i++){
    //check for updates on each button
    if(button_array[i].update()){
      //if the button was released set it to 0
      if(button_array[i].risingEdge()){
        //Send joystick release command
        //Uncomment the following line ot turn this into a joystick instead of a keyboard:
        //Joystick.button(i+1, 0); //+1 because joystick buttons are from 1-10, not 0-9.
        
        //Send keyboard release command to the key defined above
        Keyboard.release(key_array[i]);

        //Update Button State Arrays for Lighting Control:
        button_states[i] = LOW;
        button_has_triggered_lighting_event[i] = false;
        #ifdef DEBUG
          Serial.print("Released ");
          Serial.println(key_array[i]);
        #endif
      }
      //otherwise set it to pressed
      else if(button_array[i].fallingEdge()){
        //Send Joystick Press Command:
        //Uncomment the following line ot turn this into a joystick instead of a keyboard:
        //Joystick.button(i+1, 1); //+1 because joystick buttons are from 1-10, not 0-9.

        //Send keyboard press command to the key defined above.
        Keyboard.press(key_array[i]);

        //Update Button State Arrays for Lighting Control:
        button_states[i] = HIGH;
        button_has_triggered_lighting_event[i] = false;
        #ifdef DEBUG
          Serial.print("Pressed ");
          Serial.println(key_array[i]);
        #endif
      }
    }
  }
}

//This turns on a default lighting setup which will run until someone strums, then normal lighting effects will take over.
void lighting_init(){
  //Set all the LEDs to a default color.
  
  //reset the transition step variable to 0 so it will start from the new color:
  set_rainbow(current_rainbow);
  lm_current_transition_position = 0;
  LED_rainbow(lm_current_transition_position);
}

//This controls basic lighting effects based on the current state of the pressed buttons:
void lighting_control(){
  unsigned long current_time = millis();
  if((current_time - last_lighting_update) > LIGHTING_REFRESH_DELAY){
    //reset the lighting refresh timer:
    last_lighting_update = current_time;
    #ifdef LIGHTING_DEBUG
      //uncomment if needed, otherwise really annoying
      //Serial.println("Light_Update!");
    #endif
    //Standard slow-rotating rainbow
    LED_rainbow(lm_current_transition_position);
    lm_current_transition_position++;
    if(lm_current_transition_position >= LM_SLOW_ROTATE_FRAMES){
      lm_current_transition_position = 0;
    }
  }
}


//this is a general purpose function for mapping a color transition between two colors (uint32_t) based on a desired position, a starting position, and an ending position.
uint32_t color_map(int x, int in_min, int in_max, uint32_t start_color, uint32_t end_color){
  
  //assign the colors based on the above.
  uint32_t start_red = (uint8_t)(start_color >> 16);
  uint32_t start_green = (uint8_t)(start_color >> 8);
  uint32_t start_blue = (uint8_t)(start_color);
  uint32_t end_red = (uint8_t)(end_color >> 16);
  uint32_t end_green = (uint8_t)(end_color >> 8);
  uint32_t end_blue = (uint8_t)(end_color);

  //do the map thing to get the color between the two based on LM_SLOW_FADE_FRAMES
  uint32_t mid_red = map(x, in_min, in_max, start_red, end_red);
  uint32_t mid_green = map(x, in_min, in_max, start_green, end_green);
  uint32_t mid_blue = map(x, in_min, in_max, start_blue, end_blue);

  //finally assign the mid_color.
  uint32_t mid_color = strip.Color(mid_red, mid_green, mid_blue);
  return mid_color;
}

//this is a function to set a rainbow pattern around the disks with an offset:
void LED_rainbow(int offset){
  //change variables as needed for the default state of this lighting mode:
  //first need to work out the color of each pixel based on a starting offset of 0.
  //There wil be LM_SLOW_ROTATE_FRAMES states in the animation.
  //the rainbow will need to be spread evenly over these LM_SLOW_ROTATE_FRAMES, and then we will
  //need to offset by offset, which increments by 1 every frame. This will start at position 0.

  //will need to take into account negative offsets for wiki animations. This can be done by inverting the offset at the start:
  if(offset <= 0){
    offset = LM_SLOW_ROTATE_FRAMES+offset;
  }

  //So to start, we will need an offset number which maps which of the positions the num_rainbow_colors 'pure' colors map to.
  int pure_color_offset = LM_SLOW_ROTATE_FRAMES*LM_SLOW_ROTATE_SCALING_FACTOR/num_rainbow_colors;
  //then we will need an offset which notes which NUM_LEDS positions out of the total fall directly on the LEDs
  int led_position_offset = LM_SLOW_ROTATE_FRAMES*LM_SLOW_ROTATE_SCALING_FACTOR/(NUM_LEDS);
  //With this number we can make note of the positions where LEDs will always be in an array:
  int led_position_array[NUM_LEDS*2];
  led_position_array[0] = 0;
  for(int i=1; i<(NUM_LEDS*2); i++){
    led_position_array[i] = led_position_array[i-1]+led_position_offset;
  }

  uint32_t double_rainbow[num_rainbow_colors*2];
  for(int i=0; i<num_rainbow_colors; i++){
    double_rainbow[i] = rainbow[i];
  }
  for(int i=0; i<num_rainbow_colors; i++){
    double_rainbow[i+num_rainbow_colors] = rainbow[i];
  }
  
  //make an int to track the number of LEDS that have been set.
  int num_leds_set = 0;
  //and another to tell the for loop to break when all have been set
  bool all_leds_set = false;

  //iterate through the colors starting at the starting LED, and looping back when it hits NUM_LEDS
  for(int i=0; i<num_rainbow_colors*2; i++){
    //establish the current and next color. the next color will need to be color 0 on the final iteration.
    uint32_t current_color;
    uint32_t next_color;
    if(i != (num_rainbow_colors*2-1)){
      current_color = double_rainbow[i];
      next_color = double_rainbow[i+1];
    }
    else{
      current_color = double_rainbow[i];
      next_color = double_rainbow[0];
    }
    
    //set the current and next color positions. If either is greater than LM_SLOW_ROTATE_FRAMES, it will need to be fixed before setting a color.
    int current_color_position = offset*LM_SLOW_ROTATE_SCALING_FACTOR + (i*pure_color_offset);
    int next_color_position = offset*LM_SLOW_ROTATE_SCALING_FACTOR + (i*pure_color_offset) + pure_color_offset;

    //initialize for use below:
    uint32_t mid_color = 0;

    //iterate through all LEDs and see which LEDs are between the color positions.
    for(int led = 0; led < (NUM_LEDS*2); led++){
      if(led_position_array[led] >= current_color_position && led_position_array[led] < next_color_position){
        //Should only activate if the LED position is between the current and next LED.
        //set the led number to be set to a number below NUM_LEDS/2
        int corrected_led = led;
        if(led >= (NUM_LEDS)){
          corrected_led = led - (NUM_LEDS);
        }
        mid_color = color_map(led_position_array[led], current_color_position, next_color_position, current_color, next_color);
        strip_setPixelColor(corrected_led, mid_color);
        num_leds_set++;

        //check to see if it's set all of the LEDS. and if so, break from the function
        if(num_leds_set > NUM_LEDS){
          all_leds_set = true;
        }
        #ifdef DEBUG
        /* only uncomment if needed. Too spammy.
        Serial.print("i is: ");
        Serial.print(i);
        Serial.print(" and led is: ");
        Serial.print(led);
        Serial.print(" and current LED position is ");
        Serial.print(led_position_array[led]);
        Serial.print(" and current color position is ");
        Serial.println(current_color_position);
        /**/
        #endif
        
      }
      if(all_leds_set){
        break;
      }
    }
  }
  strip.show();
}

//this function switches the rainbow array to a different rainbow, as defined up top.
void set_rainbow(int num){
  num_rainbow_colors = rainbows[num].num_colors;
  for(int i=0; i<num_rainbow_colors; i++){
    rainbow[i] = rainbows[num].colors[i];
  }
  LED_rainbow(0);
}
