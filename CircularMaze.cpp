#include "CircularMaze.h"
#include <Arduino.h>
#include <math.h>
#include <vector>

// Initialize static members
//lv_point_t CircularMaze::point_buffer[CircularMaze::MAX_TOTAL_WALLS][CircularMaze::MAX_POINTS_PER_LINE];
//int CircularMaze::wall_buffer_idx = 0;

CircularMaze::CircularMaze(int rings, int sectors, int spacing) {
    NUM_RINGS = rings;
    SECTORS_PER_RING = sectors;
    RING_SPACING = spacing;
    wall_buffer_idx = 0;
    // 2. Resize the 2D vectors to match the new dimensions
    radial_walls.resize(NUM_RINGS, std::vector<bool>(SECTORS_PER_RING));
    circular_walls.resize(NUM_RINGS, std::vector<bool>(SECTORS_PER_RING));
    visited_circular.resize(NUM_RINGS, std::vector<bool>(SECTORS_PER_RING));

    // 3. Resize the LVGL point buffer
    int max_walls = (NUM_RINGS * SECTORS_PER_RING) * 2 + 1;
    point_buffer.resize(max_walls);

    // Initialize ball state for a circular maze if needed
    ballX = CENTER_X;
    ballY = CENTER_Y;
    velX = 0.0f;
    velY = 0.0f;
    lastMs = millis();
}

void CircularMaze::generate() {
    // leave extra ring room in the middle and outermost ring has no walls
    for (int r = 0; r < NUM_RINGS; r++) {
        for (int s = 0; s < SECTORS_PER_RING; s++) {
            radial_walls[r][s] = true;
            circular_walls[r][s] = true;
            visited_circular[r][s] = false;
        }
    }
    //memset(visited_circular, 0, sizeof(visited_circular));
    spawn_ring = NUM_RINGS - 1;
    spawn_sector = random(0, SECTORS_PER_RING);


    // Run DFS carve starting from ball spawn point
    carve(spawn_ring - 1, spawn_sector);
}

void CircularMaze::draw(lv_obj_t* parent, bool animate) {
    generate();
    wall_buffer_idx = 0; // Reset buffer index each time we redraw

    // LVGL One-time style init for wall lines
    static lv_style_t style_wall_circular;
    lv_style_init(&style_wall_circular);
    lv_style_set_line_width(&style_wall_circular, 2);
    lv_style_set_line_color(&style_wall_circular, lv_color_white());
    lv_style_set_line_rounded(&style_wall_circular, true);

    const float angle_step = 2 * M_PI / SECTORS_PER_RING;

    // Draw Radial Walls (Spokes)
    for (int r = 1; r < NUM_RINGS-1; r++) {
        for (int s = 0; s < SECTORS_PER_RING; s++) {
            if (r < NUM_RINGS && radial_walls[r][s]) {
                float angle = s * angle_step;
                float r1 = (r + 1) * RING_SPACING;
                float r2 = (r + 2) * RING_SPACING;

                if (wall_buffer_idx < point_buffer.size()) {
                    point_buffer[wall_buffer_idx][0] = {(lv_coord_t)(CENTER_X + cos(angle) * r1), (lv_coord_t)(CENTER_Y + sin(angle) * r1)};
                    point_buffer[wall_buffer_idx][1] = {(lv_coord_t)(CENTER_X + cos(angle) * r2), (lv_coord_t)(CENTER_Y + sin(angle) * r2)};
                    
                    lv_obj_t *wall = lv_line_create(parent);
                    lv_line_set_points(wall, point_buffer[wall_buffer_idx].data(), 2);
                    lv_obj_add_style(wall, &style_wall_circular, 0);
                    wall_buffer_idx++;
                    if (animate) lv_timer_handler();
                }
            }
        }
    }
    
    // Draw Circular Walls (Arcs)
    for (int r = 1; r < NUM_RINGS; r++) {
        for (int s = 0; s < SECTORS_PER_RING; s++) {
            if (circular_walls[r][s]) {
                if (wall_buffer_idx < point_buffer.size()) {
                    float radius = (r + 1) * RING_SPACING;
                    float start_angle = s * angle_step;
                    
                    for(int i = 0; i <= POINTS_PER_ARC; i++) {
                        float current_angle = start_angle + (float)i * (angle_step / POINTS_PER_ARC);
                        point_buffer[wall_buffer_idx][i].x = (lv_coord_t)(CENTER_X + cos(current_angle) * radius);
                        point_buffer[wall_buffer_idx][i].y = (lv_coord_t)(CENTER_Y + sin(current_angle) * radius);
                    }

                    lv_obj_t *arc_wall = lv_line_create(parent);
                    lv_line_set_points(arc_wall, point_buffer[wall_buffer_idx].data(), POINTS_PER_ARC + 1);
                    lv_obj_add_style(arc_wall, &style_wall_circular, 0);
                    wall_buffer_idx++;

                    if (animate) lv_timer_handler();
                }
            }
        }
    }

    // Draw the ball (green circle)
    lv_point_t spawn_coord = randomSpawnCoord();
    lv_obj_t* ball = lv_obj_create(parent);
    lv_obj_set_size(ball, RING_SPACING-2, RING_SPACING-2);
    lv_obj_set_pos(ball,
      spawn_coord.x - (RING_SPACING-2)/2,
      spawn_coord.y - (RING_SPACING-2)/2
    );
    lv_obj_set_style_bg_color(ball, lv_color_make(0, 255, 0), 0);
    lv_obj_set_style_border_width(ball, 0, 0);

    ballX = spawn_coord.x - (RING_SPACING-2)/2;
    ballY = spawn_coord.y - (RING_SPACING-2)/2;
    // Final actual draw to screen
    lv_timer_handler();
}

void CircularMaze::carve(int ring, int sector) {
    visited_circular[ring][sector] = true;
    int dirs[4] = {0, 1, 2, 3}; // 0:Out, 1:In, 2:CW, 3:CCW
    
    // Fisher-Yates shuffle to randomize path generation
    for (int i = 3; i > 0; i--) {
        int j = random(i + 1);
        int temp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = temp;
    }

    // Attempt to visit each neighbor in random order
    for (int i = 0; i < 4; i++) {
        int dir = dirs[i];
        int next_r = ring;
        int next_s = sector;

        if (dir == 0) next_r++;
        else if (dir == 1) next_r--;
        else if (dir == 2) next_s = (sector + 1) % SECTORS_PER_RING;
        else if (dir == 3) next_s = (sector - 1 + SECTORS_PER_RING) % SECTORS_PER_RING;
        
        // Check if the target cell is valid to move to
        if (next_r >= 0 && next_r < NUM_RINGS && !visited_circular[next_r][next_s]) {
            
            bool wall_was_removed = false;
            
            if (dir == 0) { // Moving Outward
                radial_walls[ring][sector] = false;
                wall_was_removed = true;
            } else if (dir == 1) { // Moving Inward
                radial_walls[next_r][sector] = false;
                wall_was_removed = true;
            } else if (dir >= 2) { // Moving Clockwise or Counter-Clockwise
                // This 'if' respects your design of not carving arcs on the outermost ring.
                if (ring < NUM_RINGS - 1) {
                    if (dir == 2) circular_walls[ring][sector] = false; // CW
                    else          circular_walls[ring][next_s] = false; // CCW
                    wall_was_removed = true;
                }
            }
            
            // FIX: Only recurse if a path was actually created.
            if (wall_was_removed) {
                carve(next_r, next_s);
            }
        }
    }
}


lv_point_t CircularMaze::randomSpawnCoord() {
    // Gather the list of all open sectors on the outer ring
    std::vector<int> openSectors;
    int ring = spawn_ring - 1;  // the ring index just inside the outer boundary
    for (int s = 0; s < SECTORS_PER_RING; ++s) {
        if (!radial_walls[ring][s]) {
            openSectors.push_back(s);
        }
    }

    // Pick one at random
    if (openSectors.empty()) {
        // Should never happen in a perfect maze, but fallback to spawn_sector
        openSectors.push_back(spawn_sector);
    }
    int choice = openSectors[random(0, openSectors.size())];

    // Compute the “middle of the wedge” coords exactly as before
    const float angle_step = 2.0f * M_PI / SECTORS_PER_RING;
    float angle  = (choice + 0.5f) * angle_step;
    float radius = (spawn_ring - 0.5f) * RING_SPACING;

    lv_coord_t x = CENTER_X + (lv_coord_t)(radius * cosf(angle));
    lv_coord_t y = CENTER_Y + (lv_coord_t)(radius * sinf(angle));
    return { x, y };
}

lv_point_t CircularMaze::randomPerimeterCoord() {
    // half-sector in: avoids landing on a spoke
    float angle = spawn_sector * (2.0f * M_PI / SECTORS_PER_RING);
    // half-ring in: avoids landing on an arc
    float radius = spawn_ring * RING_SPACING;

    lv_coord_t x = CENTER_X + (lv_coord_t)(radius * cos(angle));
    lv_coord_t y = CENTER_Y + (lv_coord_t)(radius * sin(angle));

    return {x, y};
}