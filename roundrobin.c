#include "roundrobin.h"

void roundRobin(list_t* processes, int quantum, char* strategy) {
    memoryList_t* memory = NULL;
    int bestFit = 0;
    if (strcmp(strategy, "best-fit") == 0) {
        memory = makeEmptyMemoryList();
        bestFit = 1;
    }
    process_t* current;
    int makespan = 0;
    list_t* ready = makeEmptyList();
    int readyNodes;
    int new = 0;
    float maxTimeOverhead = -INT_MAX;
    float totalTimeOverhead = 0;
    int turnaroundTime = 0;
    // keep running until all processes are in the finished state
    while (!isFinished(processes)) {
        // if there are any nodes in the input queue, we move them to the ready queue
        if (checkForArrived(processes, makespan)) {
            moveToReady(processes, ready, makespan, strategy, memory);
            new = 1;
        }
        readyNodes = numberOfNodes(ready);
        // if there are no processes in the ready queue, continue
        if (ready->head == NULL) {
            makespan = makespan + quantum;
            continue;
        }
        current = ready->head->process;
        // only create process when it runs for the first time
        if (current->serviceTime == current->durationLeft) {
            createProcess(current, makespan);
        } else {
            continueProcess(current, makespan);
        }
        if (current->durationLeft > quantum) {
            if (new) { 
                printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", makespan, current->processName, current->durationLeft); 
            }
            // the same process as the previous quantum, no need to print running message again
            if (readyNodes == 1) {
                new = 0;
            }
            current->durationLeft -= quantum;
            makespan += quantum;
            // insert any nodes that arrived during the quantum into the ready queue
            if (checkForArrived(processes, makespan)) {
                moveToReady(processes, ready, makespan, strategy, memory);
                new = 1;
            }
            // push current process to the back of the ready queue
            deleteNode(ready, current->processName);
            ready = insertAtFoot(ready, current);
            // if we are starting a new process, suspend the current process
            if (new) {
                suspendProcess(current, makespan);
            }
        } else {
            // if the service time is a quantum or less, it will finish
            if (new) { 
                printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", makespan, current->processName, current->durationLeft); 
            }
            current->durationLeft -= quantum;
            makespan += quantum;
            // calculating statistics
            new = 1;
            turnaroundTime += (makespan - current->timeArrived);
            float timeOverhead = (float)(makespan - current->timeArrived) / current->serviceTime;
            totalTimeOverhead += timeOverhead;
            if (timeOverhead > maxTimeOverhead) {
                maxTimeOverhead = timeOverhead;
            }
            // terminate process, and print the finish message
            deleteNode(ready, current->processName);
            // remove process from memory list if we are using best-fit
            if (bestFit) {
                removeMemoryNode(memory, current->processName);
            }
            int startRemainingProc = remainingProcesses(processes, makespan, quantum);
            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", makespan, current->processName, startRemainingProc);
            terminateProcess(current, makespan);
        }
    }
    int numProcesses = numberOfNodes(processes);
    float avgTurnaroundTime = (float)turnaroundTime / numProcesses;
    printf("Turnaround time %d\n", roundUp(avgTurnaroundTime));
    printf("Time overhead %.2f %.2f\n", maxTimeOverhead, totalTimeOverhead / numProcesses);
    printf("Makespan %d\n", makespan);
    freeList(ready);
    if (bestFit) {
        freeMemoryList(memory);
    }
}


