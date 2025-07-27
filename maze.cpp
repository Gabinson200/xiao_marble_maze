#include "maze.h"
#include <Arduino.h>
#include <math.h>

// Maze dimensions and storage (unchanged)
static const int COLS = 8;
static const int ROWS = 8;
static const int CELL = 20;
static bool horiz[ROWS+1][COLS];
static bool vert[ROWS][COLS+1];
static bool visited[ROWS][COLS];

// Maze UI
static const int MAX_WALLS = (ROWS + 1) * COLS + ROWS * (COLS + 1);
static lv_point_t wall_points[MAX_WALLS][2];
static int wall_count = 0;
int offset = 40;

// Marble and exit variables (unchanged)
static float ballX, ballY;
static float velX, velY;
static int exitR, exitC;
static uint32_t lastMs = 0;

// Forward declarations (unchanged)
static void carve(int r, int c);
static void generateMaze();
static void placeExit();


void initMaze() {
  generateMaze();
  placeExit();
  ballX = CELL/2 + offset;
  ballY = CELL/2 + offset;
  velX  = velY = 0;
  lastMs = millis();
}

// drawMaze function
void drawMaze(lv_obj_t *parent) {
  // Create a style for the white maze walls
  static lv_style_t style_wall;
  lv_style_init(&style_wall);
  lv_style_set_line_width(&style_wall, 2);
  lv_style_set_line_color(&style_wall, lv_color_white());
  lv_style_set_line_rounded(&style_wall, true);

  // Horizontal walls
  for (int r = 0; r <= ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      if (horiz[r][c]) {
         if (wall_count < MAX_WALLS) {
          wall_points[wall_count][0] = { (lv_coord_t)(c * CELL + offset), (lv_coord_t)(r * CELL + offset) };
          wall_points[wall_count][1] = { (lv_coord_t)((c + 1) * CELL + offset), (lv_coord_t)(r * CELL + offset) };
          
          lv_obj_t *wall = lv_line_create(parent);
          lv_line_set_points(wall, wall_points[wall_count], 2);
          lv_obj_add_style(wall, &style_wall, 0);
          wall_count++;
        }
        lv_timer_handler();
      }
    }
  }

  // Vertical walls 
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c <= COLS; c++) {
      if (vert[r][c]) {
        if (wall_count < MAX_WALLS) {
          wall_points[wall_count][0] = { (lv_coord_t)(c * CELL + offset), (lv_coord_t)(r * CELL + offset) };
          wall_points[wall_count][1] = { (lv_coord_t)(c * CELL + offset), (lv_coord_t)((r + 1) * CELL + offset) };

          lv_obj_t *wall = lv_line_create(parent);
          lv_line_set_points(wall, wall_points[wall_count], 2);
          lv_obj_add_style(wall, &style_wall, 0);
          wall_count++;
        }
        lv_timer_handler();
      }
    }
  }

  // Draw exit cell in green
  lv_obj_t *exit_obj = lv_obj_create(parent);
  lv_obj_set_size(exit_obj, CELL, CELL);
  lv_obj_set_pos(exit_obj, exitC * CELL, exitR * CELL);
  lv_obj_set_style_bg_color(exit_obj, lv_color_make(0,255,0), 0);
  lv_obj_set_style_border_width(exit_obj, 0, 0);
}

// Maze generation logic
static void carve(int r, int c) {
  visited[r][c] = true;
  const int dr[4] = {-1,1,0,0}, dc[4] = {0,0,-1,1};
  int dirs[4] = {0,1,2,3};
  for (int i = 3; i > 0; i--) {
    int j = random(i+1);
    int t = dirs[i];
    dirs[i] = dirs[j];
    dirs[j] = t;
  }

  for (int i = 0; i < 4; i++) {
    int d = dirs[i];
    int nr = r + dr[d];
    int nc = c + dc[d];
    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && !visited[nr][nc]) {
      if (d < 2) horiz[max(r,nr)][c] = false;
      else if (d == 2) vert[r][c] = false;
      else vert[r][c+1] = false;
      carve(nr, nc);
    }
  }
}

static void generateMaze() {
  for (int r = 0; r <= ROWS; r++)
    for (int c = 0; c < COLS; c++)
      horiz[r][c] = true;
  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c <= COLS; c++)
      vert[r][c] = true;
  memset(visited, 0, sizeof(visited));
  carve(0,0);
}

static void placeExit() {
  exitR = ROWS - 2;
  exitC = COLS - 2;
}