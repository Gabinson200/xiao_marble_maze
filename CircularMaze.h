#ifndef CIRCULAR_MAZE_H
#define CIRCULAR_MAZE_H

#include "Maze.h"

/**
 * @class CircularMaze
 * @brief Generates and draws a perfect maze on a polar (circular) grid.
 *
 * The maze consists of concentric rings subdivided into angular sectors.
 * Walls are stored separately for radial spokes and circular arcs.
 */
class CircularMaze : public Maze {
public:
    /**
     * @brief Default constructor initializes ball and timing state.
     * Walls and carving are performed in generate().
     */
    CircularMaze();

    /**
     * @brief Builds (carves) a new maze layout.
     * Uses recursive DFS starting at (spawn_ring-1, spawn_sector).
     */
    virtual void generate() override;

    /**
     * @brief Draws the maze walls and start marker on an LVGL object.
     * @param parent  Parent LVGL object for drawing lines and exit.
     * @param animate If true, renders with a brief pause per wall.
     */
    virtual void draw(lv_obj_t* parent, bool animate) override;

private:
    static constexpr int NUM_RINGS = 9;
    static constexpr int SECTORS_PER_RING = 12;
    static constexpr int RING_SPACING = 13;
    static constexpr int CENTER_X = 120;
    static constexpr int CENTER_Y = 120;
    static constexpr int POINTS_PER_ARC = 5;
    // In cirular maze random spawn location of ball on outermost - 1 ring 
    int spawn_ring; /// < Index of the outermost ring (NUM_RINGS-1)
    int spawn_sector;  ///< Sector index where entrance is carved

    bool radial_walls[NUM_RINGS][SECTORS_PER_RING];
    bool circular_walls[NUM_RINGS][SECTORS_PER_RING];
    bool visited_circular[NUM_RINGS][SECTORS_PER_RING];

    /**
     * @brief Recursively carve passages using recursive DFS.
     * @param ring   Current ring index (0 = center).
     * @param sector Current sector index.
     */
    void carve(int ring, int sector);

    /**
     * @brief Helper: returns pixel coords of the single carved entrance.
     * @return Pixel coordinates on the perimeter.
     */
    lv_point_t randomPerimeterCoord();

    /**
     * @brief Choose a random start coordinate among outer-ring openings.
     * @return Pixel coordinates centered in the chosen wedge.
     */
    lv_point_t randomSpawnCoord();

    // Pre-allocated buffers for LVGL line drawing (two endpoints or arc points)
    static constexpr int MAX_TOTAL_WALLS = (NUM_RINGS * SECTORS_PER_RING) * 2 + 1;
    static constexpr int MAX_POINTS_PER_LINE = POINTS_PER_ARC + 1;
    static lv_point_t point_buffer[MAX_TOTAL_WALLS][MAX_POINTS_PER_LINE];
    static int wall_buffer_idx; ///< Next free index in point_buffer
};

#endif // CIRCULAR_MAZE_H