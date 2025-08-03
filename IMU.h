#ifndef IMU_H
#define IMU_H

#include <LSM6DS3.h>
#include <Wire.h>

class IMU {
public:
    IMU(float threshold = 30.0f, int samples = 10);
    bool begin();
    bool read();
    void getRollAndPitch(float& roll, float& pitch) const;

private:
    LSM6DS3 myIMU;
    const float turnThreshold;
    const int numSamples;
    int samplesRead;
    float rollAvg;
    float pitchAvg;
};

#endif // IMU_H