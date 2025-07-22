#ifndef READIMU
#define READIMU

#include <LSM6DS3.h>
#include <Wire.h>


void setupIMU();

/**
* @brief Create the background objects on the passed screen which are then updated with update_background
* @param parent lv screen object on which to create the background objects 
*/
void readIMU();


#endif