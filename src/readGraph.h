#ifndef _READ_H_
#define _READ_H_

#include "tools.h"

void readMode(entryR* entry); 

node* readFromFile(entryR* entry);

void findPath(node* graph, entryR* entry);

void printShortPath(entryR* entry, int* parents);

void printExtendedPath(entryR* entry, int* parents, double* weights);

#endif
