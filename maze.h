#ifndef MAZE_H
#define MAZE_H

#include <lvgl.h>

class Ball; // have to forward declare ball class here

class Maze {
public:
    /**
     * @brief Destructor.
     */
    virtual ~Maze() {}

    // Pure virtual functions that maze subclasses must implement
    virtual void generate() = 0;

    virtual void draw(lv_obj_t* parent, bool animate) = 0;

    virtual lv_point_t getBallSpawnPixel() const { return {120,120}; }
    
    virtual lv_point_t getExitPixel() const { return {0,0}; }

    virtual void handleCollisions(Ball& ball) = 0;

    virtual void stepBallWithCollisions(Ball& ball,
                                    float max_step_px = -1.0f,
                                    uint8_t max_substeps = 32) = 0;

    // updates RTC time for maze clock, might move to maze clock class as we will probaby never have 
    // a rectangular clock maze
    virtual void updateTime() {}
};

#endif // MAZE_H