// MazeClock.h

#ifndef MAZE_CLOCK_H
#define MAZE_CLOCK_H

#include "CircularMaze.h" // Include the parent class
#include <vector>
#include <array>
#include "Ball.h"

// MazeClock inherits from CircularMaze
class MazeClock : public CircularMaze {
public:
    // fix it to 12 sectors for the 12 hours of a clock.
    MazeClock(int rings, int spacing);
  
    // Override the draw function 
    virtual void draw(lv_obj_t* parent, bool animate) override;

    virtual void updateTime() override;
private:
    lv_obj_t* hour_arc;
    lv_obj_t* minute_arc;
};

#endif // MAZE_CLOCK_H