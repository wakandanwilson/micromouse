#include <iostream>
#include <string>

#include "API.h"

const int MAZE_SIZE = 16;

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

// direction
char dir_chars[4] = {'n', 'e', 's', 'w'};

// allows you to use bitwise OR when updating which walls are present in a cell
// also allows you to bitwise AND
int dir_mask[4] = {0b1000, 0b0100, 0b0010, 0b0001};

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

enum DirectionBitmask {
    NORTH_MASK = 0b1000,
    EAST_MASK  = 0b0100,
    SOUTH_MASK = 0b0010,
    WEST_MASK  = 0b0001
};

struct Coord {
    int x;
    int y;
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
    int cellWalls[16][16]; // 2, 2D arrays for distances of each cell, and values for cell walls
    Coord* goalPos;
};

// Queue functions
void initQueue(Queue *q) { //initalize empty queue
    
}

bool isQEmpty(Queue q) {
    
}



// Maze functions
void scanWalls(Maze* maze) { // fill in code for changing value of the cell walls
  int cellWalls = 0000;
  // get dir
  if (API::wallFront()) {
    if (maze->mouse_dir == NORTH)
      cellWalls |= NORTH_MASK;
    else if (maze->mouse_dir == EAST)
      cellWalls |= EAST;
    else if (maze->mouse_dir == SOUTH)
      cellWalls |= SOUTH_MASK;
    else
      cellWalls |= WEST_MASK;
  }
  if (API::wallRight()) {
    if (maze->mouse_dir == NORTH)
      cellWalls |= EAST_MASK;
    else if (maze->mouse_dir == EAST)
      cellWalls |= SOUTH_MASK;
    else if (maze->mouse_dir == SOUTH)
      cellWalls |= WEST_MASK;
    else
      cellWalls |= NORTH_MASK;
  }
  if (API::wallLeft()) {
    if (maze->mouse_dir == NORTH)
      cellWalls |= WEST_MASK;
    else if (maze->mouse_dir == EAST)
      cellWalls |= NORTH_MASK;
    else if (maze->mouse_dir == SOUTH)
      cellWalls |= EAST_MASK;
    else
      cellWalls |= SOUTH_MASK;
  }
  
  maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x] = cellWalls;
}

void updateSimulator(Maze maze) { // redraws the maze in simulator after each loop in main
    for(int x = 0; x < MAZE_SIZE; x++) 
    {
        for(int y = 0; y < MAZE_SIZE; y++) 
        {
            if (maze.cellWalls[y][x] & NORTH_MASK) { // API set walls for some direction
                API::setWall(x, y, dir_chars[0]);
            }
            if (maze.cellWalls[y][x] & EAST_MASK) {
                API::setWall(x, y, dir_chars[1]);
            }
            if (maze.cellWalls[y][x] & SOUTH_MASK) {
                API::setWall(x, y, dir_chars[2]);
            }
            if (maze.cellWalls[y][x] & WEST_MASK){
                API::setWall(x, y, dir_chars[3]);
            }
            API::setText(x, y, std::to_string(maze.distances[x][y]));
        }
    }
}

void updateWalls(){ // put into scanWalls

}

void updateMousePos(Coord* pos, Direction dir) {
    //depending on the mouse direction, increment position by one
    if (dir == NORTH)
        // increment in some direction
        pos->y += 1;
    if (dir == SOUTH)
        // increment in some direction
        pos->y -= 1;
    if (dir == WEST)
        // increment in some direction
        pos->x -= 1;
    if (dir == EAST)
        // increment in some direction
        pos->x += 1;
}

CellList* getNeighborCells(Maze* maze, Coord c) { //to be called in a while loop within Floodfill when setting each cell
    //differentiate between accessible cells and cells that are blocked by walls
};

Cell* getBestCell(Maze*, Coord c){ // returns accessible cell with lowest distance from goal coords

}

// functions to return direction after step rotation
Direction clockwiseStep(Direction dir){ // turn right = dir + 3 % 4
    dir = Direction((dir + 3) % 4);
    return dir;
}
Direction counterClockwiseStep(Direction dir){ // turn left = dir + 1 % 4
    dir = Direction((dir + 1) % 4);
    return dir;
}

void floodFill(Maze* maze, bool to_start) { // function to be called everytime you move into a new cell
    
}

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");
    while (true) {
        if (!API::wallLeft()) {
            API::turnLeft();
        }
        while (API::wallFront()) {
            API::turnRight();
        }
        API::moveForward();
    }
}
