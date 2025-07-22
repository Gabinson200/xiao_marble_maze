#ifndef MAZE
#define MAZE

#include <LSM6DS3.h>
#include <Wire.h>

/**
* @brief Create the background objects on the passed screen which are then updated with update_background
* @param parent lv screen object on which to create the background objects 
*/
void drawMaze();


#endif