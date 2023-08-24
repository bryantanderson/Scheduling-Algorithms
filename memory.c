#include "memory.h"

// implement best-fit strategy
int allocateMemoryToProcess(process_t* process, memoryList_t* list, int makespan) {
    memory_t *curr, *prev, *new;
	int currentBestStart;
	int currentBestEnd;
	int smallestDifference = INT_MAX;
	assert(list!=NULL);
	curr = list->head;
	// first insertion into the list, so we can start the block at 0
	if (list->foot==NULL) { 
		insertInMemoryList(list, 0, 0 + process->memoryRequirement, process);
		printf("%d,READY,process_name=%s,assigned_at=%d\n", makespan, process->processName, 0);
		return 1;
	}
	// if the head of the list is a hole
	if ((list->head->start > 0) && (process->memoryRequirement <= list->head->start)) {
		new = (memory_t*)malloc(sizeof(*new));
		new->status = OCCUPIED;
		new->processName = process->processName;
		new->start = 0;
		new->end = list->head->start;
		new->next = list->head;
		new->prev = list->head->prev;
		list->head = new;
		printf("%d,READY,process_name=%s,assigned_at=%d\n", makespan, process->processName, 0);
		return 1;
	}
	// loop through the memory linked list to check for any holes
	while (curr) {
		int memoryDifference;
		prev = curr;
		curr = curr->next;
		if (prev->status == OCCUPIED) {
			continue;
		} else {
			// keep track of the node with the best fitting memory
			memoryDifference = (prev->end - prev->start) - process->memoryRequirement;
			if (memoryDifference < smallestDifference) {
				if (memoryDifference > 0) {
					smallestDifference =  memoryDifference;
					currentBestStart = prev->start;
					currentBestEnd = prev->end;
				} 
				// choose the equal sized block starting at the earlier address
				else if (memoryDifference == 0) {
					continue;
				}
			}
		}
	}
	// if we found an available hole, insert it
	if (smallestDifference != INT_MAX) {
		printf("%d,READY,process_name=%s,assigned_at=%d\n", makespan, process->processName, currentBestStart);
		insertInHole(list, currentBestStart, currentBestEnd, process);
		return 1;
	} 
	// didn't find an available hole, check if we can add a new node into the list
	else {
		if ((list->foot->end + process->memoryRequirement) <= MEMORY_LIMIT) {
			printf("%d,READY,process_name=%s,assigned_at=%d\n", makespan, process->processName, list->foot->end );
			insertInMemoryList(list, list->foot->end, 
			list->foot->end + process->memoryRequirement, process);
			return 1;
		}
	}
	// return 0 when unsuccessful allocation occurs
	return 0;
}

// deletes a memory node from the list, based on the name of the process
void removeMemoryNode(memoryList_t* list, char* processName) {
	memory_t *curr, *previous;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		previous = curr;
		curr = curr->next;
		// convert the occupied block into a hole
		if (strcmp(previous->processName, processName) == 0) {
			previous->status = HOLE;
			// make sure we are not accessing a node that does not exist
			if (curr != NULL) {
				// case where the right node is a hole
				if (curr->status == HOLE) {
					previous->end = curr->end;
					free(curr);
				}
			} 
			// make sure we are not accessing a node that does not exist
			if (previous->prev != NULL) {
				// case where the left node is a hole
				if (previous->prev->status == HOLE) {
					previous->start = previous->prev->start;
					free(previous->prev);
					return;
				}
			} 

			// when the left or right nodes are null
			if ((previous->prev == NULL) && (curr == NULL)) {
				// this node is the only node in the list
				list->head = list->foot = NULL;
				free(previous);
			} else if ((previous->prev == NULL) && (curr != NULL)) {
				// the node is the head of the list
				list->head = curr;
				curr->prev = previous->prev;
				free(previous);
			} else if ((previous->prev != NULL) && (curr == NULL)) {
				// the node is the foot of the list
				list->foot = previous->prev;
				previous->prev->next = curr;	
				free(previous);	
			}
			return; 
		}
	}
}

memoryList_t* makeEmptyMemoryList(void) {
    memoryList_t *list;
	list = (memoryList_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

void freeMemoryList(memoryList_t* list) {
    memory_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

// allocates a hole in the linked list to a process
void insertInHole(memoryList_t *list, int start, int end, process_t* process) {
	memory_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	// loop through the list until we find the hole we are looking for
	while (curr) {
		prev = curr;
		curr = curr->next;
		if ((prev->status == HOLE) && (prev->start == start) && (prev->end == end)) {
			prev->start = start;
			prev->end = end;
			prev->status = OCCUPIED;
			prev->processName = process->processName;
			process->state = READY;
			return;
		}
	}
}

// inserts a new block for a process at the end of the linked list
void insertInMemoryList(memoryList_t *list, int start, int end, process_t* process) {
    memory_t *new;
	new = (memory_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->start = start;
    new->end = end;
    new->processName = process->processName;
	new->status = OCCUPIED;
	process->state = READY;
	new->prev = list->foot;
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		new->prev = list->foot;
		list->foot->next = new;
		list->foot = new;
	}
}

// moves a process from the input to the ready queue, depending on the makespan
void moveToReady(list_t* inputQueue, list_t* readyQueue, int makespan, char* strategy, memoryList_t* memory) {
	// loop through the input queue
	node_t *curr, *prev;
	assert(readyQueue!=NULL);
	assert(inputQueue!=NULL);
	curr = inputQueue->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		// if the process has arrived, and is ready, we can move it to the ready queue
		if ((prev->process->timeArrived <= makespan) && (prev->process->state == ARRIVED)) {
			// case where we are using a best-fit memory strategy
			if (strcmp(strategy, "best-fit") == 0) {
				if (allocateMemoryToProcess(prev->process, memory, makespan)) {
					prev->process->state = READY;
					readyQueue = insertAtFoot(readyQueue, prev->process);
				}
			} 
			// case where we are using an infinite memory strategy
			else if (strcmp(strategy, "infinite") == 0) {
				prev->process->state = READY;
				readyQueue = insertAtFoot(readyQueue, prev->process);
			}
		} 
	}
}

void createProcess(process_t* process, int makespan) {
	uint8_t byte;
    int pipe1[2];
	int pipe2[2];
	pid_t pid;
	uint32_t time = htonl(makespan);
	// initialise pipes
    pipe(pipe1);
	pipe(pipe2);
	// create an instance of the process
    pid = fork();
    // if we are in the child process
    if (pid == 0) {
		close(pipe1[1]);
		dup2(pipe1[0], STDIN_FILENO);
		close(pipe2[0]);
		dup2(pipe2[1], STDOUT_FILENO);
        char* argv[] = {"./process", "-v", process->processName, NULL};
        execvp("./process", argv);
    } else if (pid > 0) {
		// if we are in the parent process
		process->pid = pid;
		close(pipe1[0]);
		close(pipe2[1]);
		write(pipe1[1], &time, sizeof(time));
		read(pipe2[0], &byte, sizeof(byte));
		// initialise the pipes of the process
		process->childPipe[0] = pipe1[0];
		process->childPipe[1] = pipe1[1];
		process->parentPipe[0] = pipe2[0];
		process->parentPipe[1] = pipe2[1];
		// compare one byte from output to the least significant byte
		if (byte == ((uint8_t*)&makespan)[0]) {
			process->state = RUNNING;
		} else {
			printf("process failed to create\n");
		}
	}

}

void terminateProcess(process_t* process, int makespan) {
	uint32_t time;
	time = htonl(makespan);
	write(process->childPipe[1], &time, sizeof(time));
	kill(process->pid, SIGTERM);
	process->state = FINISHED;
	// read in the 64 byte string SHA
	char sha[65];
    read(process->parentPipe[0], sha, 64);
    sha[64] = '\0';
    printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s\n", makespan, process->processName, sha);
}

void continueProcess(process_t* process, int makespan) {
	uint32_t time;
	uint8_t byte;
	time = htonl(makespan);
	write(process->childPipe[1], &time, sizeof(time));
	kill(process->pid, SIGCONT);
	read(process->parentPipe[0], &byte, sizeof(byte));
	// compare one byte from output to the least significant byte
	if (byte == ((uint8_t*)&makespan)[0]) {
		process->state = RUNNING;
	} 
}

void suspendProcess(process_t* process, int makespan) {
	uint32_t time;
	int waitStatus;
	time = htonl(makespan);
	write(process->childPipe[1], &time, sizeof(time));
	kill(process->pid, SIGTSTP);
	pid_t w = waitpid(process->pid, &waitStatus, WUNTRACED);
	if (WIFSTOPPED(waitStatus) && w) {
		process->state = READY;
	}
}