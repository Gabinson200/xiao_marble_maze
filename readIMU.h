#ifndef READIMU_H
#define READIMU_H

#include <LSM6DS3.h>
#include <Wire.h>

// Avergaed roll and pitch angles returned from IMU readings

void setupIMU();

/**
* @brief Updates roll and pitch angles when screen is moved 
*/
void readIMU();


#endif