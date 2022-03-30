#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <stdbool.h>

#define NO_MODE_FOUND 226
#define WRONG_POINTS 236
#define NO_COHERENT 237
#define NULL_POINTER_EXCEPTION 228
#define NO_COL_ROWS_FOUND 223
#define NO_NODES_FOUND 220

#define WEIGHT 1
#define EDGE 2
#define RANDOM 3 
#define READ 4
#define EXTENDED 5
#define STANDARD 6

typedef struct entryRead {
    int rows;
    int columns;
    char* fileName;
    short int printFlag;
    int* points;
    int numberPoints;
} entryR;

typedef struct entryGen {
    int rows;
    int columns;
    double rangeStart;
    double rangeEnd;
    short int mode;
    char* fileName;
} entryG;

typedef struct node {
    bool edgeExist[4]; 
    double edgeWeight[4]; 
    int nodeToConnect[4];
} node;

entryG* allocEntryGen(void);

entryR* allocEntryRead(entryG* entryG);

void allocPoints(char* optarg, entryR* entryR, entryG* entryG);

void freeEntryRead(entryR* entry);

void freeAll(entryR* entryR, entryG* entryG);

node* allocGraph(int numOfNodes);

bool* allocVisited(int numOfNodes);

#endif
