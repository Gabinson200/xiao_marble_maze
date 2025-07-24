#include "readIMU.h"
//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A
float gX, gY, gZ, aX, aY, aZ;
const float turnThreshold = 30;
const int numSamples = 10;
int samplesRead = numSamples;
float rollAcc, pitchAcc;
float rollSum, pitchSum, rollAvg, pitchAvg= 0;


void setupIMU(){
  if (myIMU.begin() != 0) {
      Serial.println("Device error");
    } else {
      Serial.println("gX,gY,gZ");
    }
}

void readIMU(){
  // wait for significant motion
  while (samplesRead == numSamples) {
    // read the acceleration data
    aX = myIMU.readFloatAccelX();
    aY = myIMU.readFloatAccelY();
    aZ = myIMU.readFloatAccelZ();

    gX = myIMU.readFloatGyroX();
    gY = myIMU.readFloatGyroY();
    gZ = myIMU.readFloatGyroZ();

    // sum up the absolutes
    float gSum = fabs(gX) + fabs(gY) + fabs(gZ);

    // check if it's above the threshold
    if (gSum >= turnThreshold) {
      // reset the sample read count
      samplesRead = 0;
      rollSum = 0;
      pitchSum = 0;
      pitchAvg = 0;
      rollAvg = 0;
      
      break;
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if both new acceleration and gyroscope data is
    // available
    // read the acceleration and gyroscope data

    samplesRead++;

    aX = myIMU.readFloatAccelX();
    aY = myIMU.readFloatAccelY();
    aZ = myIMU.readFloatAccelZ();

    // In radians → convert to degrees
    pitchAcc = atan2(-aX, sqrt(aY*aY + aZ*aZ)) * 180.0 / PI;  

    // Compute raw roll in degrees (–180° to +180°)
    float rawRoll = atan2(aY, aZ) * 180.0 / PI;   

    // Clamp to –90°…+90°
    float rollAcc;
    if (rawRoll > 90)      rollAcc = rawRoll - 180;
    else if (rawRoll < -90) rollAcc = rawRoll + 180;
    else                   rollAcc = rawRoll;

    rollSum += rollAcc;
    pitchSum += pitchAcc;


    /*
    Serial.print(myIMU.readFloatGyroX(), 3);
    Serial.print(',');
    Serial.print(myIMU.readFloatGyroY(), 3);
    Serial.print(',');
    Serial.print(myIMU.readFloatGyroZ(), 3);
    Serial.println();

    // Both clamped to –90°…+90°
    Serial.print(rollAcc, 1);
    Serial.print(',');
    Serial.print(pitchAcc, 1);
    Serial.println();
    */
    

    if (samplesRead == numSamples) {
      // add an empty line if it's the last sample
      rollAvg = rollSum / numSamples;
      pitchAvg = pitchSum / numSamples;
      Serial.print(rollAvg, 1);
      Serial.print(',');
      Serial.print(pitchAvg, 1);
      Serial.println();

      Serial.println();
    }
  }
}

