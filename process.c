//
// Created by xiaotian on 2021/3/15.
//
#include <stdio.h>
#include <stdlib.h>
#include "process.h"

#define MIN_INT -32767


process_t* createProcess(int pid, int arrivalTime, int executionTime, int isParallelisable, int cpuId) {

    process_t* newProcess = (process_t*) calloc(1, sizeof (process_t));
    newProcess->pid = pid;
    newProcess->arrivalTime = arrivalTime;
    newProcess->burstTime = executionTime;
    newProcess->finishingTime = 0;
    newProcess->waitingTime = 0;
    newProcess->remainingTime = executionTime;

    newProcess->isRunning = 0;
    newProcess->isParallelisable = isParallelisable;
    newProcess->cpuId = cpuId;

    return newProcess;
}

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
        } else {
            break;
        }
    }

    head->processes[i] = lastProcess;
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

process_t* FindMinRemainTimeProcess(PQ_t head) {
    if(!IsEmptyPQ(head)) {
        return head->processes[1];
    }

    printf("Empty PQ when find Min");

    return head->processes[0];
}
int IsEmptyPQ(PQ_t head) {
    return head->size == 0;
}
int IsFullPQ(PQ_t head) {
    return head->capacity == head->size;
}

int CountTotalRemainingTime(PQ_t head) {
    if(IsEmptyPQ(head)) {
        return 0;
    }

    int totalRemainingTime = 0;

    for(int i = 1; i <= head->size; i++) {
        totalRemainingTime += head->processes[i]->remainingTime;
    }

    return totalRemainingTime;
}

int IsAllCpuPQEmpty(PQ_t* cpuPQList, int numCPU) {
    for (int i = 0; i < numCPU; i++) {
        if (!IsEmptyPQ(cpuPQList[i])) {
            return 0;
        }
    }

    return 1;
}