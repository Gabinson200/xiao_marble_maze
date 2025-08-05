#include "RectangularMaze.h"
#include <Arduino.h>

RectangularMaze::RectangularMaze(int cols, int rows, int cell_size, int offset) {
    COLS = cols;
    ROWS = rows;
    CELL_SIZE = cell_size;
    OFFSET = offset;

    // Resize all wall and cell tracking vectors
    horiz_walls.resize(ROWS + 1, std::vector<bool>(COLS));
    vert_walls.resize(ROWS, std::vector<bool>(COLS + 1));
    visited_cells.resize(ROWS, std::vector<bool>(COLS));

    // 3. Resize the LVGL point buffer
    int MAX_WALLS = (ROWS + 1) * COLS + ROWS * (COLS + 1);
    wall_points.resize(MAX_WALLS);

    // Initialize ball state
    ballX = CELL_SIZE / 2.0f + OFFSET;
    ballY = CELL_SIZE / 2.0f + OFFSET;
    velX = 0.0f;
    velY = 0.0f;
    lastMs = millis();
}

void RectangularMaze::generate() {
    for (int r = 0; r <= ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) horiz_walls[r][c] = true;
    }
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS; ++c) vert_walls[r][c] = true;
    }

    // Clear the visited cells vector
    for(auto& row : visited_cells) {
        std::fill(row.begin(), row.end(), false);
    }

    carve(random(COLS), random(ROWS));
}

void RectangularMaze::draw(lv_obj_t* parent, bool animate) {
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
                if (wall_count < wall_points.size()) {
                    wall_points[wall_count][0] = { (lv_coord_t)(c * CELL_SIZE + OFFSET), (lv_coord_t)(r * CELL_SIZE + OFFSET) };
                    wall_points[wall_count][1] = { (lv_coord_t)((c + 1) * CELL_SIZE + OFFSET), (lv_coord_t)(r * CELL_SIZE + OFFSET) };

                    lv_obj_t* wall = lv_line_create(parent);
                    lv_line_set_points(wall, wall_points[wall_count].data(), 2);
                    lv_obj_add_style(wall, &style_wall, 0);
                    wall_count++;
                    if(animate) lv_timer_handler();
                }
            }
        }
    }

    //  Draw Vertical walls
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS; ++c) {
            if (vert_walls[r][c]) {
                if (wall_count < wall_points.size()) {
                    wall_points[wall_count][0] = { (lv_coord_t)(c * CELL_SIZE + OFFSET), (lv_coord_t)(r * CELL_SIZE + OFFSET) };
                    wall_points[wall_count][1] = { (lv_coord_t)(c * CELL_SIZE + OFFSET), (lv_coord_t)((r + 1) * CELL_SIZE + OFFSET) };

                    lv_obj_t* wall = lv_line_create(parent);
                    lv_line_set_points(wall, wall_points[wall_count].data(), 2);
                    lv_obj_add_style(wall, &style_wall, 0);
                    wall_count++;
                    if(animate) lv_timer_handler();
                }
            }
        }
    }

    lv_point_t spawn_coord = randomPerimeterCoord();
    // Draw Exit Cell
    lv_obj_t* ball = lv_obj_create(parent);
    lv_obj_set_size(ball, CELL_SIZE-2, CELL_SIZE-2);
    lv_obj_set_pos(ball, spawn_coord.x, spawn_coord.y);
    lv_obj_set_style_bg_color(ball, lv_color_make(0, 255, 0), 0);
    lv_obj_set_style_border_width(ball, 0, 0);

    // Final actual draw to screen
    lv_timer_handler();
}

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

lv_point_t RectangularMaze::randomPerimeterCoord() {
    int P = 2*COLS + 2*ROWS - 4;
    int idx = random(0, P);
    int r, c;
    if      (idx < COLS)        { r = 0;       c = idx;           }
    else if ((idx -= COLS) < ROWS-2) { r = 1+idx;  c = COLS-1;      }
    else if ((idx -= ROWS-2) < COLS) { r = ROWS-1; c = COLS-1-idx; }
    else                           { idx -= COLS; r = ROWS-2-idx; c = 0; }
    // return in pixels, with OFFSET
    return { (lv_coord_t)(c*CELL_SIZE + OFFSET),
             (lv_coord_t)(r*CELL_SIZE + OFFSET) };
}

void RectangularMaze::handleCollisions(Ball& ball) {
    float ball_x = ball.getX();
    float ball_y = ball.getY();
    float ball_r = ball.getRadius();

    // Convert ball's pixel coordinates to maze grid coordinates
    int col = (ball_x - OFFSET) / CELL_SIZE;
    int row = (ball_y - OFFSET) / CELL_SIZE;

    // Check for collision with the four surrounding walls
    // Top wall
    if (vert_walls[row][col] && (ball_x - ball_r < col * CELL_SIZE + OFFSET)) {
        ball.setX(col * CELL_SIZE + OFFSET + ball_r);
        ball.setVelocityX(-ball.getVelocityX() * 0.25f); // Reverse velocity and add bounce damping
    }
    // Bottom wall
    if (vert_walls[row][col + 1] && (ball_x + ball_r > (col + 1) * CELL_SIZE + OFFSET)) {
        ball.setX((col + 1) * CELL_SIZE + OFFSET - ball_r);
        ball.setVelocityX(-ball.getVelocityX() * 0.25f);
    }
    // Left wall
    if (horiz_walls[row][col] && (ball_y - ball_r < row * CELL_SIZE + OFFSET)) {
        ball.setY(row * CELL_SIZE + OFFSET + ball_r);
        ball.setVelocityY(-ball.getVelocityY() * 0.25f);
    }
    // Right wall
    if (horiz_walls[row + 1][col] && (ball_y + ball_r > (row + 1) * CELL_SIZE + OFFSET)) {
        ball.setY((row + 1) * CELL_SIZE + OFFSET - ball_r);
        ball.setVelocityY(-ball.getVelocityY() * 0.25f);
    }
}