#include <stdio.h>
#include <stdlib.h>
#include "readGraph.h"

static void saveGraphToFileRead(entryR* entry, node* graph){
    int numOfNodes = entry->columns * entry->rows;

    printf("%d %d\n",entry->rows, entry->columns);

    for (int i = 0; i < numOfNodes; i++ ){
        printf("\t");
        for (int k = 0; k < 4; k++ )
            if (graph[i].edgeExist[k] == true)
                printf(" %d :%f ",graph[i].nodeToConnect[k],graph[i].edgeWeight[k]);
        printf("\n");
    }
}

node* readFromFile(entryR* entry){
    FILE* in = fopen(entry ->fileName, "r");

    if (fscanf(in, "%d %d", &entry ->rows, &entry -> columns) != 2){
        fprintf(stderr, "ROWS AND COLUMNS NOT FOUND!\n");
        freeEntryRead(entry);
        exit(NO_COL_ROWS_FOUND);
    }

    int numOfNodes = entry ->columns * entry -> rows;
    node* graph = allocGraph(numOfNodes);

    fgetc(in);
    for (int i = 0; i < numOfNodes; i++){
        char buff[1024];
        char* buf = buff;

        if (fgets(buf,1024,in) == NULL){
            fprintf(stderr, "NODES NOT FOUND!\n");
            free(graph);
            freeEntryRead(entry);
            exit(NO_NODES_FOUND);
        }

        insertGraph(entry, graph, i, buf);
    }

    fclose(in);
    return graph;
}

void insertGraph(entryR* entry, node* graph, int i, char* buf){
    int columns = entry->columns, rows = entry->rows;
    int node = 0, offset=0;
    double weight = 0;

    while(sscanf(buf, " %d :%lf %n", &node, &weight, &offset) == 2){
        buf+=offset;

        if (i - columns >= 0 && i - columns < columns * rows){
            if (node == i - columns){
                graph[i].edgeExist[UP] = true;
                graph[i].nodeToConnect[UP] = i - columns;
                graph[i].edgeWeight[UP] = weight;
            }
        } else {
            graph[i].edgeExist[UP] = false;
        }
        if(i + 1 < columns * rows && (i+1)%columns != 0){
            if (node == i + 1){
                graph[i].edgeExist[RIGHT] = true;
                graph[i].nodeToConnect[RIGHT] = i + 1;
                graph[i].edgeWeight[RIGHT] = weight;
            }
        } else {
            graph[i].edgeExist[RIGHT] = false;
        }
        if (i + columns > 0 && i + columns < columns * rows){
            if(node == i + columns){
                graph[i].edgeExist[DOWN] = true;
                graph[i].nodeToConnect[DOWN] = i + columns;
                graph[i].edgeWeight[DOWN] = weight;
            }
        } else {
            graph[i].edgeExist[DOWN] = false;
        }
        if(i - 1 >= 0 && i%columns != 0){
            if(node == i - 1){
                graph[i].edgeExist[LEFT] = true;
                graph[i].nodeToConnect[LEFT] = i - 1;
                graph[i].edgeWeight[LEFT] = weight;
            }
        } else {
            graph[i].edgeExist[LEFT] = false;
        }
    }
}

void findPath(node* graph, entryR* entry){
    int numOfPairs = entry->numberPoints / 2;
    int numOfNodes = entry -> columns * entry -> rows;
    
    for (int i = 0; i < numOfPairs; i++){
        int startPoint = entry->points[2*i];
        int endPoint = entry->points[2*i+1];
        int currentPoint = startPoint;
        int* predecessors = allocPredecessor(numOfNodes);
        bool* known = allocVisited(numOfNodes);
        double* weights = allocWeights(numOfNodes); //przechowuje wage przejscia od poprzednika
        double* distance = allocWeights(numOfNodes); //przechowuje wage drogi do tej pory

        distance[startPoint] = 0;
        known[currentPoint] = true;
        while (true){
            for(int k = 0; k < 4; k++){
                if(graph[currentPoint].edgeExist[k]){
                    if(distance[graph[currentPoint].nodeToConnect[k]] > distance[currentPoint] + graph[currentPoint].edgeWeight[k]){
                        predecessors[graph[currentPoint].nodeToConnect[k]] = currentPoint;
                        distance[graph[currentPoint].nodeToConnect[k]] = distance[currentPoint] + graph[currentPoint].edgeWeight[k];
                        weights[graph[currentPoint].nodeToConnect[k]] = graph[currentPoint].edgeWeight[k];
                    }
                }
            }
            
            currentPoint = findNewPoint(known, distance, numOfNodes);
            if (currentPoint == -1){
                break;
            }
            known[currentPoint] = true;
        }


        if(entry->printFlag == STANDARD){
            printShortPath(entry, predecessors, startPoint, endPoint);
        }
        if ( entry->printFlag == EXTENDED) {
            printExtendedPath(entry, predecessors, weights, startPoint, endPoint);
        }

        free(predecessors);
        free(weights);
    }
}

void printShortPath(entryR* entry, int* predecessors, int startPoint, int endPoint){
    int numOfNodes = entry -> columns * entry -> rows;
    /*
    printf("----------------------\n");
    for(int i = 0; i < numOfNodes; i++){
        printf("| N: %3d ------- %3d |\n",i,predecessors[i]);
    }
    printf("----------------------\n");
    */

    int* predecessorsInOrder = allocPredecessorInOrder(numOfNodes);
    int size = 0;

    printf("(%d,%d); ", startPoint, endPoint);
    while(1) {
        predecessorsInOrder[size++] = endPoint;
        endPoint = predecessors[endPoint];
        if(endPoint == -1)
            break;
    }
    for(int i = size-1; i > 0; i--) {
        printf("%d ---> ", predecessorsInOrder[i]);
    }
    printf("%d\n", predecessorsInOrder[0]);
    free(predecessorsInOrder);
}

void printExtendedPath(entryR* entry, int* predecessors, double* weights, int startPoint, int endPoint){
    int numOfNodes = entry -> columns * entry -> rows;
    /*
    printf("----------------------\n");
    for(int i = 0; i < numOfNodes; i++){
        printf("| N: %3d ------- %3d |\n",i,predecessors[i]);
    }
    printf("----------------------\n");
    */

    int* predecessorsInOrder = allocPredecessorInOrder(numOfNodes);
    int size = 0;

    printf("(%d,%d); ", startPoint, endPoint);
    while(1) {
        predecessorsInOrder[size++] = endPoint;
        endPoint = predecessors[endPoint];
        if(endPoint == -1)
            break;
    }
    for(int i = size-1; i > 0; i--) {
        printf("%d(%f) ---> ", predecessorsInOrder[i],weights[predecessorsInOrder[i-1]]);
    }
    printf("%d\n", predecessorsInOrder[0]);
    free(predecessorsInOrder);
}

int findNewPoint(bool* known, double* distance, int numOfNodes){
    int point = -1;
    
    for (int i = 0; i < numOfNodes; i++){
        if(known[i] == false){
            point = i;
        }
    }
    if (point == -1){
        return -1;
    }
    for (int i = 0; i < numOfNodes; i++){
        if(known[i] == false && distance[i] <= distance[point]){
            point = i;
        }
    }
    return point;
}

void readMode(entryR* entry){
    node* graph = readFromFile(entry);
    int numOfNodes = entry -> columns * entry -> rows;

    printf("READ GRAPH:\n");
    saveGraphToFileRead(entry, graph);

    if (checkIfCoherent(graph, numOfNodes) != true){
        fprintf(stderr, "GRAPH IS NOT COHERENT!!!\n");
        freeEntryRead(entry);
        exit(NO_COHERENT);
    }
    else {
        printf("GRAPH IS COHERENT!!\n");
        findPath(graph, entry);
    }
    free(graph);
    freeEntryRead(entry);
}
