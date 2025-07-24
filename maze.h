#ifndef MAZE_H
#define MAZE_H

#include <lvgl.h>

/**
* @brief Create the background objects on the passed screen which are then updated with update_background
* @param parent lv screen object on which to create the background objects 
*/
// Must be called once *after* you create your LVGL canvas
// Pass the canvas object so the module can draw into it.
void setCanvas(lv_obj_t * c);

// Call in setup() to build a new maze & reset the marble.
void initMaze();

// Draws the entire maze (walls + exit) into the canvas.
void drawMaze(lv_obj_t *parent);

// Updates the marble’s position & physics based on current roll/pitch.
// roll and pitch should be in degrees (–90…+90).
void updateBall(float roll, float pitch);

// Draws (and erases previous) marble on the canvas.
void drawBall();

#endif // MAZE_H