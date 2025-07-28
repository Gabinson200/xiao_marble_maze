#ifndef READIMU_H
#define READIMU_H

#include <LSM6DS3.h>
#include <Wire.h>

// Averaged but not fused filters (may be changed later on if readings are too volatile)
extern float rollAvg, pitchAvg;

void setupIMU();

/**
* @brief If some rotational acceleration threshold is reached read in some samples from the accelerometer,
* convert to angle from the normal, and average readings for mean value. Only prints / provides new values if a disturbance is sensed. 
*/
void readIMU();


#endif