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


// LVGL Draw Buffer - small and statically allocated
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];

void setup() {
    Serial.begin(115200);

    // 1) Initialize LVGL
    lv_init();

    // 2) Initialize Display Driver
    lv_xiao_disp_init();
    //lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);

    // 2) Create & load a main screen so layers exist
    mainScreen = lv_obj_create(nullptr);
    lv_obj_set_size(mainScreen,
                    lv_disp_get_hor_res(nullptr),
                    lv_disp_get_ver_res(nullptr));
    lv_scr_load(mainScreen);

    // Initialize the IMU [cite: 4]
    Wire.begin();
    setupIMU();

    // 4) Get the main screen and set its background to black
    lv_obj_t *mainScreen = lv_scr_act();
    lv_obj_set_style_bg_color(mainScreen, lv_color_black(), 0);

    // 5) Initialize and draw the maze directly onto the screen
    //initMaze();
    //delay(3000); // this is just for when taking a video of generation
    //drawMaze(mainScreen);
    
    drawCircularMaze(mainScreen);
}

void loop() {
  readIMU();
  // updateBall(roll, pitch); 
  // drawBall();
  lv_timer_handler();
  delay(5);
}