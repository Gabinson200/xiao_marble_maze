#ifndef RECTANGULAR_MAZE_H
#define RECTANGULAR_MAZE_H

#include "Maze.h"
#include <vector>
#include <array>
#include "Ball.h"

class RectangularMaze : public Maze {
public:
    /**
     * @brief Constructor for a dynamically-sized rectangular maze.
     * @param cols The number of columns in the maze.
     * @param rows The number of rows in the maze.
     * @param cell_size The size of each cell in pixels.
     * @param offset The pixel offset from the screen edge.
     */
    RectangularMaze(int cols, int rows, int cell_size, int offset);

    virtual void generate() override;
    virtual void draw(lv_obj_t* parent, bool animate) override;

    virtual void handleCollisions(Ball& ball) override;

private:
    int COLS; // = 8;
    int ROWS; //  = 8;
    int CELL_SIZE; // = 20;
    int OFFSET; // = 40;

    std::vector<std::vector<bool>> horiz_walls;
    std::vector<std::vector<bool>> vert_walls;
    std::vector<std::vector<bool>> visited_cells;
    
    std::vector<std::array<lv_point_t, 2>> wall_points;
    int wall_count;

    void carve(int r, int c);
    lv_point_t randomPerimeterCoord();
};

#endif // RECTANGULAR_MAZE_H