#ifndef MAZE_H
#define MAZE_H

#include <lvgl.h>

class Ball; // have to forward declare ball class here

class Maze {
public:
    virtual ~Maze() {}

    // Pure virtual functions that subclasses must implement
    virtual void generate() = 0;
    virtual void draw(lv_obj_t* parent, bool animate) = 0;
    virtual void updateTime() {}

    virtual void handleCollisions(Ball& ball) = 0;

protected:
    float ballX, ballY;
    float velX, velY;
    uint32_t lastMs;
};

#endif // MAZE_H