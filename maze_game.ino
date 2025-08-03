#include <Arduino.h>
#include <lvgl.h>
#include "lv_xiao_round_screen.h"
#include "IMU.h"
#include "RectangularMaze.h"
#include "CircularMaze.h"

// Screen dimensions
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

// LVGL Draw Buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];

Maze* maze = nullptr; // Base class pointer
IMU imu;

void setup() {
    Serial.begin(115200);

    lv_init();
    lv_xiao_disp_init();
    randomSeed(analogRead(A0));

    // Create a main screen
    lv_obj_t* mainScreen = lv_obj_create(nullptr);
    lv_obj_set_size(mainScreen, lv_disp_get_hor_res(nullptr), lv_disp_get_ver_res(nullptr));
    lv_scr_load(mainScreen);
    lv_obj_set_style_bg_color(mainScreen, lv_color_black(), 0);

    // Initialize the IMU
    imu.begin();

    // Choose which maze to create
    bool create_rectangular_maze = false;
    if (create_rectangular_maze) {
        maze = new RectangularMaze();
    } else {
        maze = new CircularMaze();
    }

    // Generate and draw the maze
    if (maze) {
        maze->generate();
        maze->draw(mainScreen, true); // Animate the drawing
    }
}

void loop() {
    // Read IMU data if motion is detected
    if (imu.read()) {
        float roll, pitch;
        imu.getRollAndPitch(roll, pitch);
        
        // Update ball position based on IMU data
        if (maze) {
            // maze->updateBall(roll, pitch);
            // maze->drawBall();
        }
    }

    lv_timer_handler();
    delay(5);
}