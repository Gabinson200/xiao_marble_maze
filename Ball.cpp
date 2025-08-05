#include "Ball.h"
#include <Arduino.h> // For millis()

Ball::Ball(lv_obj_t* parent, float start_x, float start_y) {
    m_x = start_x;
    m_y = start_y;
    m_velocity_x = 0.0f;
    m_velocity_y = 0.0f;
    m_radius = 5.0f; // Ball is 10px in diameter
    m_last_update_ms = millis();

    // Create the LVGL object for the ball
    m_obj = lv_obj_create(parent);
    lv_obj_set_size(m_obj, m_radius * 2, m_radius * 2);

    static lv_style_t style_ball;
    lv_style_init(&style_ball);
    lv_style_set_bg_color(&style_ball, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_radius(&style_ball, LV_RADIUS_CIRCLE);
    lv_style_set_border_width(&style_ball, 0);
    lv_obj_add_style(m_obj, &style_ball, 0);
    
    draw(); // Draw initial position
}

void Ball::updatePhysics(float roll, float pitch) {
    // Acceleration is proportional to the tilt
    const float gravity_factor = 0.05f;
    m_velocity_x += pitch * gravity_factor;
    m_velocity_y += roll * gravity_factor;

    // Apply some friction to slow the ball down over time
    const float friction = 0.98f;
    m_velocity_x *= friction;
    m_velocity_y *= friction;
}

void Ball::move() {
    // Calculate time change for smooth movement independent of framerate
    uint32_t now = millis();
    float time_change = (now - m_last_update_ms) / 1000.0f;
    m_last_update_ms = now;

    // Update position based on velocity.
    // A scale factor is used to make movement speed reasonable.
    const float scale_factor = 10.0f; 
    m_x -= m_velocity_x * time_change * scale_factor;
    m_y -= m_velocity_y * time_change * scale_factor;
}

void Ball::draw() {
    // Update the on-screen object's position (might need to be adjusted)
    lv_obj_set_pos(m_obj, (lv_coord_t)(m_x - m_radius), (lv_coord_t)(m_y - m_radius));
}