#include <Arduino.h>
#include <lvgl.h>
#include "lv_xiao_round_screen.h"
#include "IMU.h"
#include "RectangularMaze.h"
#include "CircularMaze.h"
#include "I2C_BM8563.h"
#include "MazeClock.h"
#include "Ball.h"

// Screen dimensions
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

// LVGL Draw Buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10];

// RTC
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire);
uint32_t last_time_update = 0;

// Global objects
Maze* maze = nullptr; // Base class pointer
IMU imu;
Ball* ball = nullptr; 

enum class MazeType : uint8_t { Rectangular, Circular, Clock };

// >>> Set your choice here <<<
constexpr MazeType MazeChoice = MazeType::Circular;  // Rectangular | Circular | Clock

static Maze* createMaze(MazeType t) {
    switch (t) {
        case MazeType::Rectangular:
            // rows, cols, cell_size, offset
            return new RectangularMaze(8, 8, 20, 40);
        case MazeType::Circular:
            // rings, sectors, spacing
            return new CircularMaze(8, 12, 12);
        case MazeType::Clock:
        default:
            // hours, ring spacing
            return new MazeClock(6, 12);
    }
}

void setup() {
    Serial.begin(115200);

    Wire.begin();
    rtc.begin();

    lv_init();
    lv_xiao_disp_init();
    // Use pin noise for random number generation
    randomSeed(analogRead(A0));

    // Create a main screen
    lv_obj_t* mainScreen = lv_obj_create(nullptr);
    lv_obj_set_size(mainScreen, lv_disp_get_hor_res(nullptr), lv_disp_get_ver_res(nullptr));
    lv_scr_load(mainScreen);
    lv_obj_set_style_bg_color(mainScreen, lv_color_black(), 0);

    // Initialize the IMU
    imu.begin();

    // Choose which maze to create
    maze = createMaze(MazeChoice);

    // Generate and draw the maze
    if (maze) {
        maze->generate();
        // Animate the drawing, or set to false if you want instant maze generation
        maze->draw(mainScreen, false);

        lv_point_t spawn = maze->getBallSpawnPixel();
        Serial.print("spawn location: ");
        Serial.println(spawn.x);
        Serial.println(spawn.y);
        // choose your ball radius; if you keep default 5.0, pass that here to set the member correctly
        ball = new Ball(mainScreen, spawn.x, spawn.y, 5.0f);

    }
}

void loop() {
    float roll = 0.0f, pitch = 0.0f;

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
        imu.getRollAndPitch(roll, pitch);
    }

    // Update ball position based on IMU data
    if (ball) {
        ball->updatePhysics(roll, pitch);
        maze->stepBallWithCollisions(*ball,
            /*max_step_px=*/ ball->getRadius() * 0.5f,  // tune: smaller => safer
            /*max_substeps=*/ 24                         // cap for performance
        );
        ball->draw();
    }

    lv_timer_handler();
    delay(5);
}