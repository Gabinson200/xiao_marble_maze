#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include "lv_xiao_round_screen.h"

#include "readIMU.h"
#include "maze.h"

// Screen dimensions
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

lv_obj_t * mainScreen = NULL;

bool rect_or_circ = true;


// LVGL Draw Buffer - small and statically allocated
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];

void setup() {
    Serial.begin(115200);

    lv_init();
    lv_xiao_disp_init();
    randomSeed(analogRead(A0));

    // Create & load a main screen so layers exist
    mainScreen = lv_obj_create(nullptr);
    lv_obj_set_size(mainScreen,
                    lv_disp_get_hor_res(nullptr),
                    lv_disp_get_ver_res(nullptr));
    lv_scr_load(mainScreen);

    // Initialize the IMU 
    Wire.begin();
    setupIMU();

    // Get the main screen and set its background to black
    lv_obj_t *mainScreen = lv_scr_act();
    lv_obj_set_style_bg_color(mainScreen, lv_color_black(), 0);

    // Initialize and draw the maze directly onto the screen
    if(rect_or_circ){
      RectangularMaze maze = RectangularMaze();
      maze.draw(mainScreen, true);
    }else{
      //drawCircularMaze(mainScreen);
    }
}

void loop() {
  readIMU();
  // updateBall(roll, pitch); 
  // drawBall();
  lv_timer_handler();
  delay(5);
}