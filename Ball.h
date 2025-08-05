// Ball.h

#ifndef BALL_H
#define BALL_H

#include <lvgl.h>

class Ball {
public:
    // Constructor to create the ball's visual object and set its initial state
    Ball(lv_obj_t* parent, float start_x, float start_y);

    // Applies forces from the IMU to change the ball's velocity
    void updatePhysics(float roll, float pitch);
    
    // Updates the ball's internal x/y coordinates based on its velocity
    void move();

    // Updates the on-screen LVGL object's position to match the internal coordinates
    void draw();
    
    // --- Getters and Setters for the Maze to use ---
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    void setX(float x) { m_x = x; }
    void setY(float y) { m_y = y; }

    float getVelocityX() const { return m_velocity_x; }
    float getVelocityY() const { return m_velocity_y; }
    void setVelocityX(float vx) { m_velocity_x = vx; }
    void setVelocityY(float vy) { m_velocity_y = vy; }
    
    float getRadius() const { return m_radius; }

private:
    lv_obj_t* m_obj; // Pointer to the LVGL object for the ball

    // State variables
    float m_x;
    float m_y;
    float m_velocity_x;
    float m_velocity_y;
    
    // Properties
    float m_radius;
    uint32_t m_last_update_ms;
};

#endif // BALL_H