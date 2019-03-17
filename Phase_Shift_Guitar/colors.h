//This is just to offload color definitions to another file so I don't need to scroll so much in the main file.

#include <Adafruit_NeoPixel.h>

//this is the most numbers a 'rainbow' can have for fades and color selection.
#define MAX_RAINBOW_COLORS 27
#define MAX_NUM_RAINBOWS 10

//a structure for holding rainbows called rainbow:
struct rainbow {
  uint32_t colors[MAX_RAINBOW_COLORS];
  int num_colors;
};

//named color definitions:
uint32_t red = strip.Color(255, 0, 0);
uint32_t orange = strip.Color(255, 127, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t yellow_green = strip.Color(127, 255, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t green_blue = strip.Color(0, 255, 127);
uint32_t sky_blue = strip.Color(0, 255, 255);
uint32_t deep_blue = strip.Color(0, 127, 255);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t purple_blue = strip.Color(127, 0, 255);
uint32_t purple = strip.Color(255, 0, 255);
uint32_t dark_purple = strip.Color(255, 0, 127);

uint32_t white = strip.Color(255,255,255);
uint32_t off = strip.Color(0, 0, 0);


/* - Default rainbow template to copy.
rainbow rXX = {
  .colors = { 
    strip.Color(), 
    strip.Color(), 
    strip.Color(), 
    strip.Color(), 
    strip.Color(), 
    strip.Color()
  },
  .num_colors = 6
};
*/

//these are the various 'rainbows' that can be swapped between for color selection on rainbow functions
//rainbow1 is the traditional roygbiv rainbow pattern
rainbow r1 = {
  .colors = {
    red,
    yellow,
    green,
    sky_blue,
    blue,
    purple
  },
  .num_colors = 6
};

//rainbow r2 is a double rainbow of r1
rainbow r2 = {
  .colors = { 
    red,
    yellow,
    green,
    sky_blue,
    blue,
    purple,
    red,
    yellow,
    green,
    sky_blue,
    blue,
    purple
  },
  .num_colors = 12
};

//the primary colors red, blue and yellow:
rainbow r3 = {
  .colors = { 
    red,
    off,
    yellow, 
    off,
    blue,
    off
  },
  .num_colors = 6
};

//the secondary colors orange, green and purple:
rainbow r4 = {
  .colors = { 
    off,
    orange,
    off,
    green, 
    off,
    purple
  },
  .num_colors = 6
};

//red green and blue
rainbow r5 = {
  .colors = { 
    off,
    red,
    off,
    green, 
    off,
    blue
  },
  .num_colors = 6
};

//blue and yellow
rainbow r6 = {
  .colors = { 
    off,
    yellow,
    off,
    blue,
    off,
    yellow,
    off,
    blue
  },
  .num_colors = 8
};

//red and sky_blue
rainbow r7 = {
  .colors = { 
    off,
    red,
    off,
    sky_blue,
    off,
    red,
    off,
    sky_blue
  },
  .num_colors = 8
};

//Orange and deep_blue
rainbow r8 = {
  .colors = { 
    off,
    orange,
    off,
    deep_blue,
    off,
    orange,
    off,
    deep_blue
  },
  .num_colors = 8
};

//some colors from color guide below:
//1 - Icy Blue and White
rainbow r9 = {
  .colors = { 
    strip.Color(255,255,163), 
    strip.Color(255,180,149), 
    strip.Color(214,98,168), 
    strip.Color(106,45,138), 
    strip.Color(65,42,120), 
    strip.Color(14,13,90),
    strip.Color(255,255,163), 
    strip.Color(255,180,149), 
    strip.Color(214,98,168), 
    strip.Color(106,45,138), 
    strip.Color(65,42,120), 
    strip.Color(14,13,90)
  },
  .num_colors = 12
};
