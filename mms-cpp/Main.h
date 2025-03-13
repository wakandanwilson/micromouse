#ifndef MAIN_H_
#define MAIN_H_
#include "API.h"

#include "Structures.h"

void initQueue(Queue *q);

bool isQEmpty(Queue q);

void enqueue(Queue *q, Coord c);

Coord dequeue(Queue *q);

void initializeMaze(Maze* maze);

void scanWalls(Maze* maze);

void updateSimulator(const Maze &maze);

void updateMousePos(Coord* pos, Direction dir);

CellList* getNeighborCells(Maze* maze, Coord c);

Cell* getBestCell(Maze* maze, Coord c);

Direction clockwiseStep(Direction dir);

Direction counterClockwiseStep(Direction dir);

void moveInDirection(Maze &maze, int targetDir);

void floodFill(Maze* maze);
#endif