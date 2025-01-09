#include <iostream>
#include <string>

#include "API.h"

const int MAZE_SIZE = 16;
const int MAX_COST = 255;

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

// allows you to use bitwise OR when updating which walls are present in a cell
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

// Queue functions
void initQueue(Queue *q) { //initalize empty queue
    
}

bool isQEmpty(Queue q) {
    
}



// Maze functions
void initializeMaze(Maze* maze){
    //set all cells to 255
    for (int row = 0; row < MAZE_SIZE; row++){
        for (int col = 0; col < MAZE_SIZE; col ++){
            maze->distances[row][col] = MAX_COST;
            maze->cellWalls[row][col] = 0000;
        }
    }

    //set goal cells to 0
    maze->distances[7][7] = 0;
    maze->distances[7][8] = 0;
    maze->distances[8][7] = 0;
    maze->distances[8][8] = 0;
}

void scanWalls(Maze* maze) { // scan walls in current cell and change cellWalls value of cell

  int cellWalls = maze->cellWalls[maze->mouse_pos.row][maze->mouse_pos.col];
  
  if (API::wallFront()) {
    if (maze->mouse_dir == NORTH)
      cellWalls |= NORTH_MASK;
    else if (maze->mouse_dir == EAST)
      cellWalls |= EAST_MASK;
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
  
  maze->cellWalls[maze->mouse_pos.row][maze->mouse_pos.col] = cellWalls; // directly modifies mazes attributes based on cellWalls's new values
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

void updateMousePos(Coord* pos, Direction dir) { //depending on the mouse direction, increment position by one
    if (dir == NORTH) // increment in some direction
        pos->row += 1;
    if (dir == SOUTH) // increment in some direction
        pos->row -= 1;
    if (dir == WEST) // increment in some direction
        pos->col -= 1;
    if (dir == EAST) // increment in some direction
        pos->col += 1;
}

CellList* getNeighborCells(Maze* maze, Coord c) { //will be called in a while loop within Floodfill when setting each cell
    //differentiate between accessible cells and cells that are blocked by walls

/*  //c = coord of cell getting checked

    CellList* neighbors = new CellList; stores accessible neighbors of c
    neighbors->size = 0; set current size to 0
    neighbors->cells = new Cell[4]; max 4 neighboring cells

    //get wall value of current cell
    int currentWalls = maze->cellWalls[c.row][c.col] (gets the binary value ex: 1110)

    //maze dimensions
    int rows = MAZE_SIZE;
    int cols = MAZE_SIZE;

    //define offsets
    int rowOffsets[4] = {1, 0, -1, 0}; //north [row+1][col], east [row][col+1], south [row-1][col], west [row][col-1]
    int colOffsets[4] = {0, 1, 0, -1};
        put these coords into the queue

    //check each neighboring cell
    for (int i = 0, i < 4, i++){ 0: north, 1: east, 2: south, 3: west
        int n
    }


*/  
    
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
