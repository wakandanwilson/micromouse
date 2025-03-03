#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include "API.h"

int dir_mask[4] = {0b1000, 0b0100, 0b0010, 0b0001};

enum DirectionBitmask {
    NORTH_MASK = 0b1000,
    EAST_MASK  = 0b0100,
    SOUTH_MASK = 0b0010,
    WEST_MASK  = 0b0001
};

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

// direction
char dir_chars[4] = {'n', 'e', 's', 'w'};

// coord info [row][col]
struct Coord { 
    int row;
    int col;
};

struct Cell {
    Coord pos;
    Direction dir;
    bool blocked;
};

struct CellList {
    int size;
    Cell* cells;
};

struct Queue {
    Coord queue[255]; // Cell object array with max size
    int head = 0;
    int tail = 0; // Two int objects for head and tail
};

struct Maze {
    Coord mouse_pos;
    Direction mouse_dir;
    int distances[16][16];
    int cellWalls[16][16]; // 2 values, 1 for Manhattan distance of each cell, 1 for cellWalls of cell
    Coord* goalPos;
};

#endif