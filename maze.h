#ifndef MAZE_H
#define MAZE_H

#include <lvgl.h>


class RectangularMaze {
  public: 
    /**
    * @brief Constructor. Initializes and generates a complete maze.
    */
    RectangularMaze();

    /**
    * @brief Draws the entire maze, including walls and the exit, onto an LVGL parent object.
    * @param parent The LVGL object to draw on.
    * @param anim if True show drawing animation if not only show final product
    */
    void draw(lv_obj_t* parent, bool anim);

    float ballX, ballY;
    float velX, velY;
    uint32_t lastMs;

  private:
    // Maze Configuration (Compile-time constants by constexpr) 
    static constexpr int COLS = 8;
    static constexpr int ROWS = 8;
    static constexpr int CELL_SIZE = 20;
    static constexpr int offset = 40;

    // Maze State 
    bool horiz_walls[ROWS + 1][COLS];
    bool vert_walls[ROWS][COLS + 1];
    bool visited_cells[ROWS][COLS];
    int exit_row, exit_col;

    // LVGL UI
    static constexpr int MAX_WALLS = (ROWS + 1) * COLS + ROWS * (COLS + 1);
    lv_point_t wall_points[MAX_WALLS][2]; // Persistent buffer needed for LVGL lines
    int wall_count;

    /**
     * @brief Recursively carves paths using a depth-first search algorithm.
     * @param r The current row of the cell to carve from.
     * @param c The current column of the cell to carve from.
     */
    void carve(int r, int c);

    /**
     * @brief Initializes the maze grid with all walls present and starts the carving process.
     */
    void generateMaze();


    /**
     * @brief Returns random perimeter position of a ROWS by COLS maze 
     */
    lv_point_t randomPerimeterCoord();


};


/**
* @brief draws the maze to the passed screen, by creating and drawing walls depending on the vert / horiz wall list produced by 
* @param parent lv screen object on which to create the maze  
*/
void drawMaze(lv_obj_t *parent);

// Updates the marble’s position & physics based on current roll/pitch.
// roll and pitch should be in degrees (–90…+90).
void updateBall(float roll, float pitch);

// Draws (and erases previous) marble on the canvas.
void drawBall();

/**
 * @brief Generates a circular maze using a recursive backtracking algorithm.
 *
 * This function initializes the maze structure with all walls present and then
 * calls the recursive 'carveCircular' function to create paths.
 */
void generateCircularMaze();

/**
 * @brief Draws the generated circular maze on an LVGL parent object.
 * @param parent The LVGL object to draw the maze on.
 */
void drawCircularMaze(lv_obj_t *parent);

#endif // MAZE_H