#include "maze.h"
#include <Arduino.h>
#include <math.h>


RectangularMaze::RectangularMaze()
    :wall_count(0) {

    generateMaze();
    
    // Initialize ball state
    // offset of 40 is needed so that square does not go outside of round display
    ballX = CELL_SIZE / 2.0f + 40;
    ballY = CELL_SIZE / 2.0f + 40;
    velX = 0.0f;
    velY = 0.0f;
    lastMs = millis();
}


void RectangularMaze::draw(lv_obj_t* parent, bool anim) {

    // This style is static, so it's initialized only once across all instances.
    static lv_style_t style_wall;
    static bool style_initialized = false;
    if (!style_initialized) {
        lv_style_init(&style_wall);
        lv_style_set_line_width(&style_wall, 2);
        lv_style_set_line_color(&style_wall, lv_color_white());
        lv_style_set_line_rounded(&style_wall, true);
        style_initialized = true;
    }
    // remove any existing walls/exit
    lv_obj_clean(parent);
    wall_count = 0; // Reset for redraw

    // Draw Horizontal walls
    for (int r = 0; r <= ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (horiz_walls[r][c]) {
                if (wall_count < MAX_WALLS) {
                    wall_points[wall_count][0] = { (lv_coord_t)(c * CELL_SIZE + offset), (lv_coord_t)(r * CELL_SIZE + offset) };
                    wall_points[wall_count][1] = { (lv_coord_t)((c + 1) * CELL_SIZE + offset), (lv_coord_t)(r * CELL_SIZE + offset) };

                    lv_obj_t* wall = lv_line_create(parent);
                    lv_line_set_points(wall, wall_points[wall_count], 2);
                    lv_obj_add_style(wall, &style_wall, 0);
                    wall_count++;
                    if(anim) lv_timer_handler();
                }
            }
        }
    }

    //  Draw Vertical walls
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS; ++c) {
            if (vert_walls[r][c]) {
                if (wall_count < MAX_WALLS) {
                    wall_points[wall_count][0] = { (lv_coord_t)(c * CELL_SIZE + offset), (lv_coord_t)(r * CELL_SIZE + offset) };
                    wall_points[wall_count][1] = { (lv_coord_t)(c * CELL_SIZE + offset), (lv_coord_t)((r + 1) * CELL_SIZE + offset) };

                    lv_obj_t* wall = lv_line_create(parent);
                    lv_line_set_points(wall, wall_points[wall_count], 2);
                    lv_obj_add_style(wall, &style_wall, 0);
                    wall_count++;
                    if(anim) lv_timer_handler();
                }
            }
        }
    }

    lv_point_t exit_coord = randomPerimeterCoord();
    // Draw Exit Cell
    lv_obj_t* exit_obj = lv_obj_create(parent);
    lv_obj_set_size(exit_obj, CELL_SIZE, CELL_SIZE);
    lv_obj_set_pos(exit_obj, exit_coord.x, exit_coord.y);
    lv_obj_set_style_bg_color(exit_obj, lv_color_make(0, 255, 0), 0);
    lv_obj_set_style_border_width(exit_obj, 0, 0);

    // Final actual draw to screen
    lv_timer_handler();
    
}

void RectangularMaze::generateMaze() {
    for (int r = 0; r <= ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            horiz_walls[r][c] = true;
        }
    }
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS; ++c) {
            vert_walls[r][c] = true;
        }
    }
    memset(visited_cells, 0, sizeof(visited_cells));
    carve(random(COLS), random(ROWS)); // Start carving from top-left
}


lv_point_t RectangularMaze::randomPerimeterCoord(){
    int P = 2*COLS + 2*ROWS - 4;
    int idx = random(0, P);
    int r, c;
    if      (idx < COLS)        { r = 0;       c = idx;           }
    else if ((idx -= COLS) < ROWS-2) { r = 1+idx;  c = COLS-1;      }
    else if ((idx -= ROWS-2) < COLS) { r = ROWS-1; c = COLS-1-idx; }
    else                           { idx -= COLS; r = ROWS-2-idx; c = 0; }
    // return in pixels, with offset
    return { (lv_coord_t)(c*CELL_SIZE + offset),
             (lv_coord_t)(r*CELL_SIZE + offset) };
}

// Maze generation logic
void RectangularMaze::carve(int r, int c) {
  visited_cells[r][c] = true;
  const int dr[4] = {-1,1,0,0}, dc[4] = {0,0,-1,1};
  int dirs[4] = {0,1,2,3};
  // Fisher-Yates shuffle
  for (int i = 3; i > 0; i--) {
    int j = random(i+1);
    int t = dirs[i];
    dirs[i] = dirs[j];
    dirs[j] = t;
  }
  // might switch to stack implementation rather than recursive
  for (int i = 0; i < 4; i++) {
    int d = dirs[i];
    int nr = r + dr[d];
    int nc = c + dc[d];
    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && !visited_cells[nr][nc]) {
      if (d < 2) horiz_walls[max(r,nr)][c] = false;
      else if (d == 2) vert_walls[r][c] = false;
      else vert_walls[r][c+1] = false;
      carve(nr, nc);
    }
  }
}




// =================================================================
// == Circular Maze Code (Corrected)                          ==
// =================================================================

// --- Circular Maze Definitions ---
const int NUM_RINGS = 10;
const int SECTORS_PER_RING = 18;
const int RING_SPACING = 11;
const int CENTER_X = 120;
const int CENTER_Y = 120;
const int POINTS_PER_ARC = 5; // Number of segments to approximate a curve

// --- Circular Maze Storage ---
bool radial_walls[NUM_RINGS][SECTORS_PER_RING];
bool circular_walls[NUM_RINGS][SECTORS_PER_RING];
bool visited_circular[NUM_RINGS][SECTORS_PER_RING];

// --- Persistent Point Buffer for LVGL ---
// Calculate max possible walls to size the point buffer correctly.
const int MAX_RADIAL_WALLS = NUM_RINGS * SECTORS_PER_RING;
const int MAX_CIRCULAR_WALLS = NUM_RINGS * SECTORS_PER_RING;
const int MAX_BOUNDARY_POINTS = SECTORS_PER_RING + 1;
const int MAX_TOTAL_WALLS_CIRCULAR = MAX_RADIAL_WALLS + MAX_CIRCULAR_WALLS + 1; // +1 for boundary

// The maximum number of points needed for any single line object (an arc or the boundary).
const int MAX_POINTS_PER_LINE = (POINTS_PER_ARC > MAX_BOUNDARY_POINTS) ? POINTS_PER_ARC + 1 : MAX_BOUNDARY_POINTS;

// A single, static buffer to hold the points for ALL line objects in the circular maze.
// This is crucial because LVGL needs the point data to persist in memory.
static lv_point_t circular_maze_point_buffer[MAX_TOTAL_WALLS_CIRCULAR][MAX_POINTS_PER_LINE];
static int circular_wall_buffer_idx = 0; // Index for the next wall in the buffer

// --- Forward Declarations for Circular Maze ---
void carveCircular(int ring, int sector);
void generateCircularMaze();

/**
 * @brief Generates a circular maze using a recursive backtracking algorithm.
 */
void generateCircularMaze() {
    // leave extra ring room in the middle
    for (int r = 2; r < NUM_RINGS; r++) {
        for (int s = 0; s < SECTORS_PER_RING; s++) {
            radial_walls[r][s] = true;
            circular_walls[r][s] = true;
        }
    }
    memset(visited_circular, 0, sizeof(visited_circular));
    carveCircular(0, random(SECTORS_PER_RING));
}

/**
 * @brief Recursively carves paths in the circular maze.
 */
void carveCircular(int ring, int sector) {
    visited_circular[ring][sector] = true;
    int dirs[] = {0, 1, 2, 3}; // 0:Out, 1:In, 2:CW, 3:CCW
    for (int i = 3; i > 0; i--) {
        int j = random(i + 1);
        int temp = dirs[i]; dirs[i] = dirs[j]; dirs[j] = temp;
    }

    for (int i = 0; i < 4; i++) {
        int dir = dirs[i];
        int next_r = ring;
        int next_s = sector;

        if (dir == 0) next_r++;
        if (dir == 1) next_r--;
        if (dir == 2) next_s = (sector + 1) % SECTORS_PER_RING;
        if (dir == 3) next_s = (sector - 1 + SECTORS_PER_RING) % SECTORS_PER_RING;

        if (next_r >= 0 && next_r < NUM_RINGS - 1 && !visited_circular[next_r][next_s]) {
            if (dir == 0) radial_walls[ring][sector] = false;
            if (dir == 1) radial_walls[next_r][sector] = false;
            if (dir == 2) circular_walls[ring][sector] = false;
            if (dir == 3) circular_walls[ring][next_s] = false;
            carveCircular(next_r, next_s);
        }
    }
}

/**
 * @brief Draws the generated circular maze on an LVGL parent object.
 */
void drawCircularMaze(lv_obj_t *parent) {
    generateCircularMaze();
    circular_wall_buffer_idx = 0; // Reset buffer index each time we redraw

    static lv_style_t style_wall_circular;
    lv_style_init(&style_wall_circular);
    lv_style_set_line_width(&style_wall_circular, 2);
    lv_style_set_line_color(&style_wall_circular, lv_color_white());
    lv_style_set_line_rounded(&style_wall_circular, true);

    const float angle_step = 2 * M_PI / SECTORS_PER_RING;

    // --- Draw Radial Walls (Spokes) ---
    for (int r = 0; r < NUM_RINGS; r++) {
        for (int s = 0; s < SECTORS_PER_RING; s++) {
            if (r < NUM_RINGS - 1 && radial_walls[r][s]) {
                float angle = s * angle_step;
                float r1 = (r + 1) * RING_SPACING;
                float r2 = (r + 2) * RING_SPACING;

                if (circular_wall_buffer_idx < MAX_TOTAL_WALLS_CIRCULAR) {
                    circular_maze_point_buffer[circular_wall_buffer_idx][0] = {(lv_coord_t)(CENTER_X + cos(angle) * r1), (lv_coord_t)(CENTER_Y + sin(angle) * r1)};
                    circular_maze_point_buffer[circular_wall_buffer_idx][1] = {(lv_coord_t)(CENTER_X + cos(angle) * r2), (lv_coord_t)(CENTER_Y + sin(angle) * r2)};
                    
                    lv_obj_t *wall = lv_line_create(parent);
                    lv_line_set_points(wall, circular_maze_point_buffer[circular_wall_buffer_idx], 2);
                    lv_obj_add_style(wall, &style_wall_circular, 0);
                    circular_wall_buffer_idx++;
                    lv_timer_handler();
                }
            }
        }
    }
    
    // --- Draw Circular Walls (Arcs) ---
    for (int r = 0; r < NUM_RINGS; r++) {
        for (int s = 0; s < SECTORS_PER_RING; s++) {
            if (circular_walls[r][s]) {
                if (circular_wall_buffer_idx < MAX_TOTAL_WALLS_CIRCULAR) {
                    float radius = (r + 1) * RING_SPACING;
                    float start_angle = s * angle_step;
                    
                    for(int i = 0; i <= POINTS_PER_ARC; i++) {
                        float current_angle = start_angle + (float)i * (angle_step / POINTS_PER_ARC);
                        circular_maze_point_buffer[circular_wall_buffer_idx][i].x = (lv_coord_t)(CENTER_X + cos(current_angle) * radius);
                        circular_maze_point_buffer[circular_wall_buffer_idx][i].y = (lv_coord_t)(CENTER_Y + sin(current_angle) * radius);
                    }

                    lv_obj_t *arc_wall = lv_line_create(parent);
                    lv_line_set_points(arc_wall, circular_maze_point_buffer[circular_wall_buffer_idx], POINTS_PER_ARC + 1);
                    lv_obj_add_style(arc_wall, &style_wall_circular, 0);
                    circular_wall_buffer_idx++;

                    lv_timer_handler();
                }
            }
        }
    }

   
}


