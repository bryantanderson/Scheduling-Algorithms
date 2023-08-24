#ifndef _SHORTESTJOBFIRST_H_
#define _SHORTESTJOBFIRST_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include "linkedlist.h"
#include "memory.h"

void shortestJobFirst(list_t* processes, int quantum, char* strategy);

process_t* currentShortestJob(list_t* processes, int makespan);

#endif