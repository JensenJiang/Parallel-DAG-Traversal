#ifndef SRC_BUILDDAG_H_
#define SRC_BUILDDAG_H_

#include "config.h"
int rand_int(int range = 128);
void dataComplete(const char *filename);

void buildDAG2(const char *filename, Graph *graph);
void buildDAG (const char *filename, Graph *graph);

int tmain(int argc, char **argv);

#endif