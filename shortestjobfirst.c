#include "shortestjobfirst.h"

void shortestJobFirst(list_t* processes, int quantum, char* strategy) {
    list_t* ready = makeEmptyList();
    memoryList_t* memory = NULL;
    int bestFit = 0;
    if (strcmp(strategy, "best-fit") == 0) {
        memory = makeEmptyMemoryList();
        bestFit = 1;
    }
    process_t* current;
    int makespan = 0;
    int turnaroundTime = 0;
    float maxTimeOverhead = -INT_MAX;
    float totalTimeOverhead = 0;
    int startRemainingProc;
    // keep running until all processes are in the finished state
    while (!isFinished(processes)) {
        // if there are any nodes in the input queue, we move them to the ready queue
        if (checkForArrived(processes, makespan)) {
            moveToReady(processes, ready, makespan, strategy, memory);
        }
        current = currentShortestJob(ready, makespan);
        // if there are no processes in the ready queue, continue
        if (current == NULL) {
            makespan = makespan + quantum;
            continue;
        }
        createProcess(current, makespan);
        int duration = current->serviceTime;
        printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", makespan, current->processName, current->serviceTime);
        while (current->state == RUNNING) {
            if (duration > quantum) {
                duration -= quantum;
                makespan += quantum;
                // include whatever processes arrived during the quantum
                if (checkForArrived(processes, makespan)) {
                    moveToReady(processes, ready, makespan, strategy, memory);
                }
                continueProcess(current, makespan);
            } else {
                duration -= quantum;
                makespan += quantum;
                // calculation of statistics
                startRemainingProc = remainingProcesses(processes, makespan, quantum);
                turnaroundTime += (makespan - current->timeArrived);
                float timeOverhead = (float)(makespan - current->timeArrived) / current->serviceTime;
                totalTimeOverhead += timeOverhead;
                if (timeOverhead > maxTimeOverhead) {
                    maxTimeOverhead = timeOverhead;
                }
                // remove process from memory list if we are using best-fit
                if (bestFit) {
                    removeMemoryNode(memory, current->processName);
                }
                // delete node from ready queue and print FINISHED message
                deleteNode(ready, current->processName);
                printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", makespan, current->processName, startRemainingProc);
                terminateProcess(current, makespan);
            }
        }
    }
    // final statistic calculations and printing 
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

// this function finds the process with the shortest service time 
process_t* currentShortestJob(list_t* processes, int makespan) {
    node_t *curr, *prev;
    process_t* shortest;
    int shortestTime = INT_MAX;
	assert(processes!=NULL);
	curr = processes->head;
    // looping through the list
	while (curr) {
		prev = curr;
		curr = curr->next;
        // if the process is finished, we move onto other processes
        if (prev->process->state == FINISHED) {
            continue;
        }
        // the process has not yet arrived, we cannot run it
        if (prev->process->timeArrived > makespan) {
            continue;
        }
        // case where the process has a shorter service time than the current shortest process
        if (prev->process->serviceTime < shortestTime) {
            shortestTime = prev->process->serviceTime;
            shortest = prev->process;
        }
        // service time tie, so we compare using arrival times
        else if (prev->process->serviceTime == shortestTime) {
            if (prev->process->timeArrived < shortest->timeArrived) {
                shortestTime = prev->process->serviceTime;
                shortest = prev->process;
            }
        }
	}

    return shortest;
}
