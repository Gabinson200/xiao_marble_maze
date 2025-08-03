#ifndef RECTANGULAR_MAZE_H
#define RECTANGULAR_MAZE_H

#include "Maze.h"

class RectangularMaze : public Maze {
public:
    RectangularMaze();
    virtual void generate() override;
    virtual void draw(lv_obj_t* parent, bool animate) override;

private:
    static constexpr int COLS = 8;
    static constexpr int ROWS = 8;
    static constexpr int CELL_SIZE = 20;
    static constexpr int OFFSET = 40;

    bool horiz_walls[ROWS + 1][COLS];
    bool vert_walls[ROWS][COLS + 1];
    bool visited_cells[ROWS][COLS];
    
    static constexpr int MAX_WALLS = (ROWS + 1) * COLS + ROWS * (COLS + 1);
    lv_point_t wall_points[MAX_WALLS][2];
    int wall_count;

    void carve(int r, int c);
    lv_point_t randomPerimeterCoord();
};

#endif // RECTANGULAR_MAZE_H