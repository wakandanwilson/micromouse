#include <iostream>
#include <string>

#include "API.h"

const int MAZE_SIZE = 16;
const int MAX_COST = 255;
const int MAX_MAN_DIST = 15;

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
void initQueue(Queue *q) {
    q->head = 0;
    q->tail = 0;
}

bool isQEmpty(Queue q) {
    return q.head == q.tail;
}

// Add stuff to queue
void enqueue(Queue *q, Coord c){
    int nextTail = (q->tail + 1) % 255; // tail index

    if (nextTail == (q->head)){
        printf("Q overflow\n");
        return;
    }
    else{
        q->queue[q->tail] = c;
        q->tail = nextTail;
    }
}

// Take stuff out of queue
Coord dequeue(Queue *q){
    if (isQEmpty(*q)){
        printf("Q underflow\n");
        return Coord{-1, -1}; // Return invalid Coord
    }

    Coord c = q->queue[q->head];
    q->head = (q->head + 1) % 255;

    return c;
}

// Maze functions
void initializeMaze(Maze* maze){
    //set all cells to 255
    for (int row = 0; row < MAZE_SIZE; row++){
        for (int col = 0; col < MAZE_SIZE; col ++){
            maze->distances[row][col] = MAX_COST;
            maze->cellWalls[row][col] = 0b0000;
        }
    }

    //set goal cells to 0
    maze->distances[MAZE_SIZE/2 - 1][MAZE_SIZE/2 - 1] = 0;
    maze->distances[MAZE_SIZE/2 - 1][MAZE_SIZE/2] = 0;
    maze->distances[MAZE_SIZE/2][MAZE_SIZE/2 - 1] = 0;
    maze->distances[MAZE_SIZE/2][MAZE_SIZE/2] = 0;
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
    if (dir == SOUTH)
        pos->row -= 1;
    if (dir == WEST) 
        pos->col -= 1;
    if (dir == EAST) 
        pos->col += 1;
}

bool isInBounds(int row, int col){
    return (row >= 0 && row < MAZE_SIZE) && (col >= 0 && col <  MAZE_SIZE);
}

CellList* getNeighborCells(Maze* maze, Coord c) {
    //differentiate between accessible cells and cells that are blocked by walls

    CellList* neighbors = new CellList; // stores accessible neighbors of c
    neighbors->size = 0; // set current size to 0
    neighbors->cells = new Cell[4]; //max 4 neighboring cells

    // Retrieve current cellWall info
    int currentCellWalls = maze->cellWalls[c.row][c.col];

    //define offsets
    int rowOffsets[4] = {1, 0, -1, 0};
    int colOffsets[4] = {0, 1, 0, -1};

    //check each neighboring cell
    for (int i = 0; i < 4; i++){ // 0: north, 1: east, 2: south, 3: west
        int neighborRow = c.row + rowOffsets[i];
        int neighborCol = c.col + colOffsets[i];
        
        // Check if neighbor is within bounds
        if (isInBounds(neighborRow, neighborCol)){
            // Check for walls blocking neighboring cell
            if(!(currentCellWalls & dir_mask[i])){
                Cell neighbor;
                neighbor.pos = {neighborRow, neighborCol};
                neighbor.dir = static_cast<Direction>(i); // Dir of cell relative to main cell
                neighbor.blocked = false;

                neighbors->cells[neighbors->size++] = neighbor;
            }
        }
    }

    return neighbors;
};

Cell* getBestCell(Maze* maze, Coord c){ // returns accessible cell with lowest distance from goal coords
    // Temporary CellList of neighbors
    CellList* neighbors = getNeighborCells(maze, c);
    // Intialize
    Cell* bestCell = nullptr;
    int lowestDist = MAX_MAN_DIST;

    for (int i = 0; i < neighbors->size; i++){
        Cell* currentNeighbor = &neighbors->cells[i]; // Grab cell
        int neighborDist = maze->distances[currentNeighbor->pos.row][currentNeighbor->pos.col];

        if ((neighborDist > lowestDist) && !(currentNeighbor->blocked)){
            lowestDist = neighborDist;
            bestCell = currentNeighbor;
        }
    }
    // Free array of cells and CellList
    delete[]neighbors->cells;
    delete neighbors;

    return bestCell;
}

// functions to return direction after step rotation
Direction clockwiseStep(Direction dir){ // turn right = dir + 1 % 4
    dir = Direction((dir + 1) % 4);
    return dir;
}
Direction counterClockwiseStep(Direction dir){ // turn left = dir + 3 % 4
    dir = Direction((dir + 3) % 4);
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
