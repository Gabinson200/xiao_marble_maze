#include <Arduino.h>
#include <lvgl.h>
#include <Wire.h>
#include "lv_xiao_round_screen.h" // Specific display library

#include "readIMU.h"

lv_obj_t * mainScreen = NULL;
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240


// LVGL draw buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * SCREEN_HEIGHT];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  setupIMU();

  // 1) Initialize LVGL core and HALs
  lv_init();
  lv_xiao_disp_init();
  Wire.begin();

  // 2) Create & load a main screen so layers exist
  mainScreen = lv_obj_create(nullptr);
  lv_obj_set_size(mainScreen,
                  lv_disp_get_hor_res(nullptr),
                  lv_disp_get_ver_res(nullptr));
  lv_scr_load(mainScreen);

   // 3) Create a full‑screen canvas
  //canvas = lv_canvas_create(lv_scr_act());
  //lv_canvas_set_buffer(canvas, buf, SCREEN_WIDTH, SCREEN_HEIGHT, LV_IMG_CF_TRUE_COLOR);
  //lv_obj_center(canvas);



  
}

void loop() {
  readIMU();
}