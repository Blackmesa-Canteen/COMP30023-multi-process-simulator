//
// Created by xiaotian on 2021/3/15.
//
#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "hashtable.h"

#define MIN_INT 0

/* process struct is used to form up a priority queue to simulate a cpu's process stack */
process_t* createProcess(unsigned int pid, unsigned int arrivalTime, unsigned int executionTime, int isParallelisable, int cpuId, int subProcNo) {

    process_t* newProcess = (process_t*) calloc(1, sizeof (process_t));
    if(newProcess == NULL) {
        printf("createProcess malloc error");
        exit(1);
    }
    newProcess->pid = pid;
    newProcess->arrivalTime = arrivalTime;
    newProcess->burstTime = executionTime;
    newProcess->finishingTime = 0;
    newProcess->waitingTime = 0;
    newProcess->remainingTime = executionTime;

    newProcess->isRunning = 0;
    newProcess->isParallelisable = isParallelisable;
    newProcess->cpuId = cpuId;
    newProcess->subProcNo = subProcNo;

    return newProcess;
}

/* init a priority queue to simulate a cpu's process stack */
PQ_t InitializePQ(int numElements, int cpuId) {
    if(numElements == 0) {
        printf("Error in numElements in Initialize method");
        return NULL;
    }
    PQ_t head = NULL;
    head = (struct Heap*) calloc(1, sizeof (struct Heap));
    if (head == NULL) {
        printf("Error allocating Heap in Initialize method \n");
        return NULL;
    }

    head->processes = (process_t**) calloc(numElements + 1 , sizeof (process_t*));
    if(head->processes == NULL) {
        printf("Error allocating processes in Initialize method \n");
        return NULL;
    }

    head->capacity = numElements;
    head->size = 0;
    head->cpuId = cpuId;

    // init dummyHead's remaining time to the smallest int
    process_t* dummyHead = (process_t*) calloc(1 ,sizeof (process_t));
    dummyHead->remainingTime = MIN_INT;
    head->processes[0] = dummyHead;

    return head;
}

/* add a process to the priority queue */
void InsertPQ(process_t* newProcess, PQ_t head) {
    int i = 0;
    if(IsFullPQ(head)) {
        printf("PQ is full, insert method error \n");
        return;
    }

    // find the right position to insert
    for( i = head->size + 1; head->processes[i/2]->remainingTime > newProcess->remainingTime; i /= 2) {
        head->processes[i] = head->processes[i/2];
    }

    head->processes[i] = newProcess;
    head->size++;
}

/* pop up the process that has the shortest remaining time from the priority queue */
process_t* DeleteMinRemainTimeProcess(PQ_t head) {
    int child = 0, i = 0;
    process_t* minRemainTimeProcess;
    process_t* lastProcess;

    if(IsEmptyPQ(head)) {
        printf("PQ is empty, Err in DeleteMin method \n");
        return head->processes[0]; // return dummyHead
    }
    minRemainTimeProcess = head->processes[1];
    lastProcess = head->processes[head->size];
    head->size -= 1;

    // loop to find the smallest remain-time child
    for (i = 1; (2 * i <= head->size); i = child) {
        child = 2 * i;
        if (child != head->size && ((head->processes[child + 1]->remainingTime) < (head->processes[child]->remainingTime))) {
            // find smaller between 2
            child++;
        }

        if (lastProcess->remainingTime > head->processes[child]->remainingTime) {
            head->processes[i] = head->processes[child];
        } else if (lastProcess->remainingTime < head->processes[child]->remainingTime) {
            break;
        } else {
//            for(i = 1; i < head->size; i++) {
//                process_t * temp = NULL;
//                temp = head->processes[i];
//                head->processes[i] = head->processes[i+1];
//                head->processes[i+1] = temp;
//            }
//            head->processes[i] = lastProcess;
//            return minRemainTimeProcess;
            break;
        }
    }

    head->processes[i] = lastProcess;

    // add pid sorting
    for (i = 1; (2 * i <= head->size); i = child) {
        process_t* temp = NULL;
        child = 2 * i;
        if (head->processes[i]->remainingTime == head->processes[child]->remainingTime) {
            if(head->processes[i]->pid > head->processes[child]->pid) {
                // head->processes[i] = head->processes[child];
                temp = head->processes[i];
                head->processes[i] = head->processes[child];
                head->processes[child] = temp;

            }
        }

        if (head->processes[child]->remainingTime == head->processes[child+1]->remainingTime) {
            if(head->processes[child]->pid > head->processes[child+1]->pid) {
                // head->processes[i] = head->processes[child];
                temp = head->processes[child];
                head->processes[child] = head->processes[child+1];
                head->processes[child+1] = temp;
            }
        }
    }

    return minRemainTimeProcess;
}


void DestroyPQ(PQ_t head) {

    free(head->processes[0]);
    free(head->processes);
    free(head);
    head = NULL;
}

void DropPQ(PQ_t head) {
    head->size = 0;
}


/* find the remaining time one from pq */
process_t* FindMinRemainTimeProcess(PQ_t head) {
    if(!IsEmptyPQ(head)) {
        return head->processes[1];
    }

    printf("Empty PQ when find Min");

    return NULL;
}
int IsEmptyPQ(PQ_t head) {
    return head->size == 0;
}
int IsFullPQ(PQ_t head) {
    return head->capacity == head->size;
}

/* count the cpu core pq's all process remaining time */
unsigned int CountTotalRemainingTime(PQ_t head) {
    if(IsEmptyPQ(head)) {
        return 0;
    }

    unsigned int totalRemainingTime = 0;

    for(int i = 1; i <= head->size; i++) {
        totalRemainingTime += head->processes[i]->remainingTime;
    }

    return totalRemainingTime;
}

int CountAllProcesses(PQ_t* cpuPQList, int numCPU) {
    HashTable countTable;
    ht_setup(&countTable, sizeof (unsigned int), sizeof (int), 512);
    ht_reserve(&countTable, 512 * 2);
    int res = 0;

    for(int i = 0; i < numCPU; i++) {
        PQ_t cpuPQ = cpuPQList[i];
        if (IsEmptyPQ(cpuPQ)) {
            continue;
        }

        for(int j = 1; j <= cpuPQList[i]->size; j++) {
            if(cpuPQList[i]->processes[j]->remainingTime != 0) {
                if(cpuPQList[i]->processes[j]->isParallelisable == 0) {
                    res++;
                } else {
                    if(!ht_contains(&countTable, &(cpuPQList[i]->processes[j]->pid))) {
                        res++;
                        ht_insert(&countTable, &(cpuPQList[i]->processes[j]->pid), &res);
                    }
                }

            }
        }


    }
    ht_clear(&countTable);
    ht_destroy(&countTable);
    return res;
}


int IsAllCpuPQEmpty(PQ_t* cpuPQList, int numCPU) {
    for (int i = 0; i < numCPU; i++) {
        if (!IsEmptyPQ(cpuPQList[i])) {
            return 0;
        }
    }

    return 1;
}