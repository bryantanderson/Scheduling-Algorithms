#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ARRIVED -2
#define READY -1
#define RUNNING 0
#define FINISHED 1

typedef struct node node_t;

typedef struct process {
    int timeArrived;
    char* processName;
    int serviceTime;
    int memoryRequirement;
    int state;
    int durationLeft;
    int childPipe[2];
    int parentPipe[2];
    pid_t pid;
} process_t;

struct node {
	process_t *process;
	node_t *next;
    node_t *prev;
};

typedef struct list{
	node_t *head;
	node_t *foot;
} list_t;

typedef struct memory memory_t;

struct memory {
	char* processName;
    // use status to check whether a memory node is occupied by a process or free
    int status;
    int start;
    int end;
	memory_t *next;
    memory_t *prev;
};

typedef struct memoryList {
	memory_t *head;
    memory_t *foot;
} memoryList_t;

list_t *makeEmptyList(void);

int numberOfNodes(list_t *list);

int isFinished(list_t *list);

int isEmptyList(list_t *list);

void freeList(list_t *list);

list_t *insertAtHead(list_t *list, process_t *value);

list_t *insertAtFoot(list_t *list, process_t *value);

int checkForArrived(list_t* list, int makespan);

void deleteNode(list_t *list, char *processName);

int remainingProcesses(list_t* processes, int makespan, int quantum);

int roundUp(float x);

#endif