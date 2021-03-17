//
// Created by xiaotian on 2021/3/15.
//

#ifndef COMP30023_2021_PROJECT_1_PROCESS_H
#define COMP30023_2021_PROJECT_1_PROCESS_H
struct Process{

    int pid;
    int arrivalTime;
    int burstTime;
    int finishingTime;
    int waitingTime;
    int remainingTime;

    int isFinished;
    int isParallelisable;
};

typedef struct Process process_t;

// Using binary-heap to represent the CPU with processes
struct Heap {
    int size;
    int capacity;
    int cpuId;
    process_t** processes;
};
typedef struct Heap* PQ_t;

process_t* createProcess(int pid, int arrivalTime, int executionTime, int isParallelisable);
PQ_t InitializePQ(int numElements, int cpuId);
void DestroyPQ(PQ_t head);
void DropPQ(PQ_t head);
void InsertPQ(process_t* newProcess, PQ_t head);
process_t* DeleteMinRemainTimeProcess(PQ_t head);
process_t* FindMinRemainTimeProcess(PQ_t head);
int IsEmptyPQ(PQ_t head);
int IsFullPQ(PQ_t head);

#endif //COMP30023_2021_PROJECT_1_PROCESS_H
