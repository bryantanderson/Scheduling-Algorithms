#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include "linkedlist.h"

#define MEMORY_LIMIT 2048
#define OCCUPIED 1
#define HOLE 0

memoryList_t* makeEmptyMemoryList(void);

void freeMemoryList(memoryList_t* list);

void removeMemoryNode(memoryList_t* list, char* processName);

int allocateMemoryToProcess(process_t* process, memoryList_t* list, int makespan);

void insertInHole(memoryList_t *list, int start, int end, process_t* process);

void insertInMemoryList(memoryList_t *list, int start, int end, process_t* process);

void moveToReady(list_t* inputQueue, list_t* readyQueue, int makespan, char* strategy, memoryList_t* memory);

void createProcess(process_t* process, int makespan);

void terminateProcess(process_t* process, int makespan);

void continueProcess(process_t* process, int makespan);

void suspendProcess(process_t* process, int makespan);

#endif