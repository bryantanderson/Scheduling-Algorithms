#include "shortestjobfirst.h"
#include "roundrobin.h"
#include "linkedlist.h"
#include <string.h>

#define IMPLEMENTS_REAL_PROCESS

int main(int argc, char* argv[]) {
    char* filename;
    char* scheduler;
    char* strategy;
    int quantum, opt;
    // parse the arguments from the command line and read them into variables
    while ((opt = getopt(argc, argv, "f:s:m:q:")) != -1) {
        switch(opt) {
            case 'f':   
                filename = strdup(optarg);
                break;
            case 's':        
                scheduler = strdup(optarg);
                break;
            case 'm':         
                strategy = strdup(optarg);
                break;
            case 'q':
                quantum = atoi(optarg);
                break;
        }
    }

    // Open file
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int timeArrived, serviceTime, memoryRequirement;
    char processName[10];
    list_t* input = makeEmptyList();
    
    // read in the information about the processes
    while (fscanf(fp, "%d %s %d %d ", &timeArrived, processName, &serviceTime, &memoryRequirement) == 4) {
        process_t* newProcess = (process_t *) malloc(sizeof(process_t));
        newProcess->timeArrived = timeArrived;
        newProcess->processName = strdup(processName);
        newProcess->serviceTime = serviceTime;
        newProcess->durationLeft = serviceTime;
        newProcess->memoryRequirement = memoryRequirement;
        newProcess->state = ARRIVED;
        input = insertAtFoot(input, newProcess);
    }
    // shortest job first 
    if ((strcmp(scheduler, "SJF") == 0)){
        shortestJobFirst(input, quantum, strategy);
    }

    // round robin 
    if ((strcmp(scheduler, "RR") == 0)){
        roundRobin(input, quantum, strategy);
    }   
    
    freeList(input);   
    free(filename);
    free(scheduler);
    free(strategy);
    fclose(fp); 
    return 0;
}