#include "linkedlist.h"
#include <stdlib.h>
#include <assert.h>

list_t* makeEmptyList(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

int isEmptyList(list_t *list) {
	assert(list!=NULL);
	return list->head==NULL;
}

void freeList(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
        free(prev->process->processName);
		free(prev->process);
		free(prev);
	}
	free(list);
}

// returns the number of nodes in the linked list
int numberOfNodes(list_t *list) {
	node_t *curr;
	assert(list!=NULL);
	curr = list->head;
	int n = 0;
	while (curr) {
		curr = curr->next;
		n++;
	}
	return n;
}

// checks whether all processes in the list are finished. if all are finished, returned 1, otherwise return 0
int isFinished(list_t *list) {
	int unfinished = 0;
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		if (prev->process->state != FINISHED) {
			unfinished++;
		}
	}
	if (unfinished == 0) {
		return 1;
	} else {
		return 0;
	}
}

list_t* insertAtHead(list_t *list, process_t *value) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->process = value;
	new->next = list->head;
	new->prev = NULL;
	list->head = new;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->foot = new;
	}
	return list;
}

list_t* insertAtFoot(list_t *list, process_t *value) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->process = value;
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
	return list;
}


// for debugging, prints all the contents of the processes in the given list
void printAllNodes(list_t *list) {
    node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		printf("%d %s %d %d\n", prev->process->timeArrived, prev->process->processName, 
        prev->process->serviceTime, prev->process->memoryRequirement);
	}
}

// loops through the list to check whether there are any new input processes
int checkForArrived(list_t* list, int makespan) {
	node_t *curr, *prev;
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		if (prev->process->state == ARRIVED && (prev->process->timeArrived <= makespan)) {
			return 1;
		}
	}
	return 0;
}

// deletes a node from the given list, based on the name of the process
void deleteNode(list_t *list, char *processName) {
    node_t *curr;
	assert(list != NULL);
	curr = list->head;
    while (curr != NULL) {
        if (strcmp(processName, curr->process->processName) == 0) {
            break;
        }
        curr = curr->next;
    }
    // if node was found, remove it from the list
    if (curr != NULL) {
        // if node is the head of the list
        if (curr == list->head) {
            list->head = curr->next;
        }

        // if node is the foot of the list
        if (curr == list->foot) {
            list->foot = curr->prev;
        }
        // update the previous node's next pointer
        if (curr->prev != NULL) {
            curr->prev->next = curr->next;
        }

        // update the next node's previous pointer
        if (curr->next != NULL) {
            curr->next->prev = curr->prev;
        }
    }
	free(curr);
}

// checks how many processes are left in the input or ready queues
int remainingProcesses(list_t* processes, int makespan, int quantum) {
    node_t *curr, *prev;
    int remainingProcesses = 0;
    assert(processes!=NULL);
	curr = processes->head;
    while (curr) {
		prev = curr;
		curr = curr->next;
        // a process is remaining if it is in the input or ready queue
        if (prev->process->timeArrived < (makespan - quantum) && (prev->process->state == READY || prev->process->state == ARRIVED)) {
            remainingProcesses++;
        }
    }
    return remainingProcesses;
}

// rounds up a float
int roundUp(float x) {
   int integerPart = (int)x;  
   float decimalPart = x - integerPart; 
   if (decimalPart > 0) {
      return integerPart + 1; 
   } else {
      return integerPart;  
   }
}


