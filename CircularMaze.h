#ifndef CIRCULAR_MAZE_H
#define CIRCULAR_MAZE_H

#include "Maze.h"
#include <vector>
#include <array>
#include "Ball.h"


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
     * @brief Constructor that accepts dynamic maze dimensions.
     * @param rings The number of concentric rings.
     * @param sectors The number of sectors per ring.
     * @param spacing The pixel spacing between each ring.
     */
    CircularMaze(int rings, int sectors, int spacing);

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

    lv_point_t getBallSpawnPixel() const override { return ball_spawn_px; }
    lv_point_t getExitPixel() const override { return exit_px; }

    virtual void handleCollisions(Ball& ball) override;

    virtual void stepBallWithCollisions(Ball& ball,
                                    float max_step_px = -1.0f,
                                    uint8_t max_substeps = 32) override;

    

protected:
    int NUM_RINGS; // = 9;
    int SECTORS_PER_RING; // = 12;
    int RING_SPACING; // = 13;
    static constexpr int CENTER_X = 120;
    static constexpr int CENTER_Y = 120;
    static constexpr int POINTS_PER_ARC = 5;
    // In cirular maze random spawn location of ball on outermost - 1 ring 
    int spawn_ring; /// < Index of the outermost ring (NUM_RINGS-1)
    int spawn_sector;  ///< Sector index where entrance is carved

    std::vector<std::vector<bool>> radial_walls;
    std::vector<std::vector<bool>> circular_walls;
    std::vector<std::vector<bool>> visited_circular;

    // Exit spawn coord vars
    int exit_sector = 0;
    lv_point_t exit_px = {0,0};
    lv_point_t ball_spawn_px = {CENTER_X, CENTER_Y};

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

    void pickExitAndSpawnOpposite();

    /**
     * @brief Choose a random start coordinate among outer-ring openings.
     * @return Pixel coordinates centered in the chosen wedge.
     */
    lv_point_t randomSpawnCoord();

    void initDebugStyles();

    // Pre-allocated buffers for LVGL line drawing (two endpoints or arc points)
    //static constexpr int MAX_TOTAL_WALLS = (NUM_RINGS * SECTORS_PER_RING) * 2 + 1;
    static constexpr int MAX_POINTS_PER_LINE = POINTS_PER_ARC + 1;
    std::vector<std::array<lv_point_t, MAX_POINTS_PER_LINE>> point_buffer;
    //static lv_point_t point_buffer[MAX_TOTAL_WALLS][MAX_POINTS_PER_LINE];
    int wall_buffer_idx; ///< Next free index in point_buffer

    // LVGL debug objects:
    lv_style_t _dbgStyleTest, _dbgStyleHit;
    lv_obj_t*  _dbgLabel = nullptr;
    lv_obj_t* _drawParent = nullptr;
  
};

#endif // CIRCULAR_MAZE_H