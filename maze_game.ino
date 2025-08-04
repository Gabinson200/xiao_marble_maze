#include <Arduino.h>
#include <lvgl.h>
#include "lv_xiao_round_screen.h"
#include "IMU.h"
#include "RectangularMaze.h"
#include "CircularMaze.h"
#include "I2C_BM8563.h"
#include "MazeClock.h"

// Screen dimensions
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

// LVGL Draw Buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];

I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
uint32_t last_time_update = 0;

Maze* maze = nullptr; // Base class pointer
IMU imu;

void setup() {
    Serial.begin(115200);

    Wire.begin();
    rtc.begin();

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
        maze = new RectangularMaze(8, 8, 20, 40);
    } else {
        //maze = new CircularMaze(9, 12, 13);
        maze = new MazeClock(6, 12);
    }

    // Generate and draw the maze
    if (maze) {
        maze->generate();
        maze->draw(mainScreen, true); // Animate the drawing
    }
}

void loop() {
    // Check if 60 seconds have passed since the last update
    if (millis() - last_time_update > 60000) {
        last_time_update = millis();
        if (maze) {
            maze->updateTime(); // Call the new update function
            Serial.println("Time updated");
        }
    }

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