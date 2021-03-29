//
// Created by xiaotian on 2021/3/15.
//

#ifndef COMP30023_2021_PROJECT_1_PROCESS_H
#define COMP30023_2021_PROJECT_1_PROCESS_H
struct Process{

    unsigned int pid;
    unsigned int arrivalTime;
    unsigned int burstTime;
    unsigned int finishingTime;
    unsigned int waitingTime;
    unsigned int remainingTime;

    int isRunning;
    int isParallelisable;
    int cpuId;
    int subProcNo;
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

process_t* createProcess(unsigned int pid, unsigned int arrivalTime, unsigned int executionTime, int isParallelisable, int cpuId, int subProcNo);
PQ_t InitializePQ(int numElements, int cpuId);
void DestroyPQ(PQ_t head);
void DropPQ(PQ_t head);
void InsertPQ(process_t* newProcess, PQ_t head);
process_t* DeleteMinRemainTimeProcess(PQ_t head);
process_t* FindMinRemainTimeProcess(PQ_t head);
int IsEmptyPQ(PQ_t head);
int IsFullPQ(PQ_t head);
int isFindProcessPid(PQ_t head, unsigned int pid);

unsigned int CountTotalRemainingTime(PQ_t head);
int IsAllCpuPQEmpty(PQ_t* cpuPQList, int numCPU);
int CountAllProcesses(PQ_t* cpuPQList, int numCPU);

#endif //COMP30023_2021_PROJECT_1_PROCESS_H
