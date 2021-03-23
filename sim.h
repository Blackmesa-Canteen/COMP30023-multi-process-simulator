//
// Created by Shaotien Lee on 2021/3/23.
//

#ifndef COMP30023_2021_PROJECT_1_SIM_H
#define COMP30023_2021_PROJECT_1_SIM_H
#include "process.h"
#include "myHashtable.h"

void SimRun(input_node_ptr input_list_head, int numMainProcess, int numCPU, int useOwnScheduler);
input_node_ptr ReadFile(const char *fileName, FILE *fp);
unsigned int RoundToInt(double f);
double RoundToTwoDigit(double f);
int CountLines(FILE *fp, int lineCounter);
void
GetArgs(int argc, char **argv, char **charPart, int argChar, int *useOwnScheduler, int *numProcessors, char **fileName);

void HandleOneCPU(input_node_ptr input_list_head, int numMainProcess, input_node_ptr input_process_ptr,
                  unsigned int *globalTimer, unsigned int *totalDeltaTime, int *numRunnedProcesses,
                  double *maxTimeOverhead, double *totalTimeOverhead);

void HandleMultiCPU(input_node_ptr input_list_head, int numMainProcess, int numCPU, HashTable *parallelProcTable,
                    input_node_ptr input_process_ptr, unsigned int *globalTimer, unsigned int *totalDeltaTime,
                    int *numRunnedProcesses, double *maxTimeOverhead, double *totalTimeOverhead);


process_t *
HandleArrivingProcOnOneCpu(input_node_ptr input_list_head, input_node_ptr input_process_ptr, unsigned int *globalTimer,
                           unsigned int *totalDeltaTime, int *numRunnedProcesses, double *maxTimeOverhead,
                           double *totalTimeOverhead, PQ_t pq, process_t *minRemainTimeProcess_ptr);

void HandleRemainProcOnOneCPU(unsigned int *globalTimer, unsigned int *totalDeltaTime, int *numRunnedProcesses,
                              double *maxTimeOverhead, double *totalTimeOverhead, PQ_t pq,
                              process_t *minRemainTimeProcess_ptr);

void InitMemCpuForMultiProc(int numMainProcess, int numCPU, HashTable *parallelProcTable, PQ_t **cpuPQList,
                            process_t ***cpuMinProcess_ptr_list);

void HandleArrivingProcOnMultiCpu(input_node_ptr input_list_head, int numCPU, HashTable *parallelProcTable,
                                  input_node_ptr input_process_ptr, unsigned int *globalTimer,
                                  unsigned int *totalDeltaTime, int *numRunnedProcesses, double *maxTimeOverhead,
                                  double *totalTimeOverhead, int procCounter, PQ_t *cpuPQList,
                                  process_t **cpuMinProcess_ptr_list);

void HandleRemainProcOnMultiCPU(int numCPU, HashTable *parallelProcTable, unsigned int *globalTimer,
                                unsigned int *totalDeltaTime, int *numRunnedProcesses, double *maxTimeOverhead,
                                double *totalTimeOverhead, int procCounter, PQ_t *cpuPQList,
                                process_t **cpuMinProcess_ptr_list);
#endif //COMP30023_2021_PROJECT_1_SIM_H
