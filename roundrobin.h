#ifndef _ROUNDROBIN_H_
#define _ROUNDROBIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include "linkedlist.h"
#include "memory.h"

void roundRobin(list_t* processes, int quantum, char* strategy);

void allocateMemory(list_t* processes);

#endif 