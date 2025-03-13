#include <iostream>
#include <string>

#include "API.h"
#include "Main.h"
#include "Constants.h"
#include "Structures.h"

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

/* Queue functions */
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

/* Maze functions */
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
    int row = maze->mouse_pos.row;
    int col = maze->mouse_pos.col;

    if (API::wallFront()) {
        if (maze->mouse_dir == NORTH){
            cellWalls |= NORTH_MASK;
            if (row + 1 < MAZE_SIZE){
                maze->cellWalls[row + 1][col] |= SOUTH_MASK;
            }
        }
        else if (maze->mouse_dir == EAST){
            cellWalls |= EAST_MASK;
            if (col + 1 < MAZE_SIZE){
                maze->cellWalls[row][col+1] |= WEST_MASK;
            }
        }
        else if (maze->mouse_dir == SOUTH){
            cellWalls |= SOUTH_MASK;
            if (row - 1 >= 0){
                maze->cellWalls[row-1][col] |= NORTH_MASK;
            }
        }
        else{
            cellWalls |= WEST_MASK;
            if (col - 1 >= 0){
                maze->cellWalls[row][col-1] |= EAST_MASK;
            }
        }
    }   

    if (API::wallRight()) {
        if (maze->mouse_dir == NORTH){
            cellWalls |= EAST_MASK;
            if (col + 1 < MAZE_SIZE){
                maze->cellWalls[row][col + 1] |= WEST_MASK;
            }
        }
        else if (maze->mouse_dir == EAST){
            cellWalls |= SOUTH_MASK;
            if (row - 1 >= 0){
                maze->cellWalls[row - 1][col] |= NORTH_MASK;
            }
        }
        else if (maze->mouse_dir == SOUTH){
            cellWalls |= WEST_MASK;
            if (col - 1 >= 0){
                maze->cellWalls[row][col - 1] |= EAST_MASK;
            }
        }
        else{
            cellWalls |= NORTH_MASK;
            if (row + 1 < MAZE_SIZE) {
                maze->cellWalls[row + 1][col] |= SOUTH_MASK;
            }
        }
    }
    if (API::wallLeft()) {
        if (maze->mouse_dir == NORTH){
            cellWalls |= WEST_MASK;
            if (col - 1 >= 0){
                maze->cellWalls[row][col - 1] |= EAST_MASK;
            }
        }
        else if (maze->mouse_dir == EAST){
            cellWalls |= NORTH_MASK;
            if (row + 1 < MAZE_SIZE){
                maze->cellWalls[row + 1][col] |= SOUTH_MASK;
            }
        }
        else if (maze->mouse_dir == SOUTH){
            cellWalls |= EAST_MASK;
            if (col + 1 < MAZE_SIZE){
                maze->cellWalls[row][col + 1] |= WEST_MASK;
            }
        }
        else{
            cellWalls |= SOUTH_MASK;
            if (row - 1 >= 0){
                maze->cellWalls[row - 1][col] |= NORTH_MASK;
            }
        }
    }

  
    maze->cellWalls[maze->mouse_pos.row][maze->mouse_pos.col] = cellWalls; // directly modifies mazes attributes based on cellWalls's new values
}

void updateSimulator(const Maze &maze) {// redraws the maze in simulator after each loop in main
    for (int row = 0; row < MAZE_SIZE; row++) {
        for (int col = 0; col < MAZE_SIZE; col++) {
            // Check each wall using the correct row and col
            if (maze.cellWalls[row][col] & NORTH_MASK) {
                API::setWall(col, row, dir_chars[0]);
            }
            if (maze.cellWalls[row][col] & EAST_MASK) {
                API::setWall(col, row, dir_chars[1]);
            }
            if (maze.cellWalls[row][col] & SOUTH_MASK) {
                API::setWall(col, row, dir_chars[2]);
            }
            if (maze.cellWalls[row][col] & WEST_MASK) {
                API::setWall(col, row, dir_chars[3]);
            }
            // Use row for y and col for x when displaying distances
            //std::cerr << maze.mouse_pos.row << std::endl;
            //std::cerr << maze.mouse_pos.col << std::endl;
            API::setText(col, row, std::to_string(maze.distances[row][col]));
        }
    }
}

void updateMousePos(Coord* pos, Direction dir) { //depending on the mouse direction, increment position by one
    switch (dir){
        case NORTH:
            pos->row++;
            break;
        case SOUTH:
            pos->row--;
            break;
        case WEST:
            pos->col--;
            break;
        case EAST:
            pos->col++;
            break;
    }
}

/* FloodFill helper functions */
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
        if ((neighborRow >= 0 && neighborRow < MAZE_SIZE) && (neighborCol >= 0 && neighborCol <  MAZE_SIZE)){
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
    int lowestDist = MAX_COST;

    for (int i = 0; i < neighbors->size; i++){
        Cell* currentNeighbor = &neighbors->cells[i]; // Grab cell
        int neighborDist = maze->distances[currentNeighbor->pos.row][currentNeighbor->pos.col];

        if ((neighborDist < lowestDist) && !(currentNeighbor->blocked)){
            lowestDist = neighborDist;
            bestCell = currentNeighbor;
        }
    }
    // Free memory of cells and CellList
    delete[]neighbors->cells;
    delete neighbors;

    return bestCell;
}

/* Step/move functions */
Direction clockwiseStep(Direction dir){ // turn right = dir + 1 % 4
    dir = Direction((dir + 1) % 4);
    return dir;
}

Direction counterClockwiseStep(Direction dir){ // turn left = dir + 3 % 4
    dir = Direction((dir + 3) % 4);
    return dir;
}

void moveInDirection(Maze &maze, int targetDir){
    int dir_diff = (targetDir - maze.mouse_dir + 4) % 4;
    switch (dir_diff){
        case 0: // bestCell is in front
            API::moveForward();
            break;
        case 1: // right
            API::turnRight();
            maze.mouse_dir = clockwiseStep(maze.mouse_dir);
            API::moveForward();
            break;
        case 2: // behind
            API::turnLeft();
            maze.mouse_dir = counterClockwiseStep(maze.mouse_dir);
            API::turnLeft();
            maze.mouse_dir = counterClockwiseStep(maze.mouse_dir);
            API::moveForward();
            break;
        case 3: // left
            API::turnLeft();
            maze.mouse_dir = counterClockwiseStep(maze.mouse_dir);
            API::moveForward();
            break;
    }
    updateMousePos(&maze.mouse_pos, maze.mouse_dir);
}

/* FloodFill */
void floodFill(Maze* maze) { // function to be called everytime you move into a new cell
    // initialize queue and goal cells
    Queue q;
    initQueue(&q);
    for (int row = 0; row < MAZE_SIZE; row++){
        for (int col = 0; col < MAZE_SIZE; col ++){
            maze->distances[row][col] = MAX_COST;
        }
    }

    maze->distances[MAZE_SIZE/2 - 1][MAZE_SIZE/2 - 1] = 0;
    enqueue(&q, {MAZE_SIZE/2 - 1, MAZE_SIZE/2 - 1});
    maze->distances[MAZE_SIZE/2 - 1][MAZE_SIZE/2] = 0;
    enqueue(&q, {MAZE_SIZE/2 - 1, MAZE_SIZE/2});
    maze->distances[MAZE_SIZE/2][MAZE_SIZE/2 - 1] = 0;
    enqueue(&q, {MAZE_SIZE/2, MAZE_SIZE/2 - 1});
    maze->distances[MAZE_SIZE/2][MAZE_SIZE/2] = 0;
    enqueue(&q, {MAZE_SIZE/2, MAZE_SIZE/2});

    while(!isQEmpty(q)){
        Coord current = dequeue(&q);
        CellList* neighborCells = getNeighborCells(maze, current);

        for (int i = 0; i < neighborCells->size; i++){
            Cell& neighbor = neighborCells->cells[i];
            Coord neighborCoord = neighbor.pos;

            int newDist = maze->distances[current.row][current.col] + 1;

            if (!(neighbor.blocked)){
                if (maze->distances[neighborCoord.row][neighborCoord.col] > newDist){
                    maze->distances[neighborCoord.row][neighborCoord.col] = newDist;
                    enqueue(&q, neighborCoord);
                }
            }
        }
        delete[] neighborCells->cells;
        delete neighborCells;
    }
}

int main(int argc, char* argv[]) {
    log("Running...");
    API::setColor(0, 0, 'G');
    API::setText(0, 0, "abc");

    Maze maze;
    initializeMaze(&maze);

    maze.mouse_pos.row = 0;
    maze.mouse_pos.col = 0;
    maze.mouse_dir = NORTH;

    while (true) {
        scanWalls(&maze);
        floodFill(&maze);
        updateSimulator(maze);

        Cell* bestCell = getBestCell(&maze, maze.mouse_pos);
        moveInDirection(maze, bestCell->dir);
    }
    return 0;
}
