#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "readFile.h"
#include "linkedlist.h"

#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define MAXLENGTH 513
#define MAX_INT 2147483647

void SimRun(input_node_ptr input_list_head, int numMainProcess, int numCPU, int useOwnScheduler);
input_node_ptr ReadFile(const char *fileName, FILE *fp);
int RoundToInt(float f);
float RoundToTwoDigitFloat(float f);
int CountLines(FILE *fp, int lineCounter);
void
GetArgs(int argc, char **argv, char **charPart, int argChar, int *useOwnScheduler, int *numProcessors, char **fileName);

int main(int argc, char** argv) {

    int useOwnScheduler = 0;
    char* charPart = NULL;
    int numProcessors = 1;
    char* fileName = NULL;
    int argChar = 0;
    int lineCounter = 0;
    FILE *fp = NULL;
    opterr = 0;

    /* Handle args */
    GetArgs(argc, argv, &charPart, argChar, &useOwnScheduler, &numProcessors, &fileName);

    /* Open the input file with the given filename for reading
     * for file : time-arrived, process-id, execution-time, parallelisable
     */
    fp = fopen(fileName, "r");
    if (!fp) {
        printf("can't opening file '%s'\n", fileName);
        return -1;
    }
    /* count file lines (total processes) */
    lineCounter = CountLines(fp, lineCounter);

    /* read the file, and store them in the linked list */
    input_node_ptr input_list_head = ReadFile(fileName, fp);

    /* run simulation */
    SimRun(input_list_head, lineCounter, numProcessors, useOwnScheduler);

    return 0;
}

void GetArgs(int argc, char **argv, char **charPart, int argChar, int *useOwnScheduler, int *numProcessors,
        char **fileName) {// handle args
    while ((argChar = getopt (argc, argv, "cp:f:")) != -1)
        switch (argChar)
        {
            case 'c':
                (*useOwnScheduler) = 1;
                break;
            case 'p':
                (*numProcessors) =  (int)strtol(optarg, charPart, 10);
                if(strlen((*charPart)) != 0 || ((*numProcessors) <= 0 || (*numProcessors) > 1024)) {
                    fprintf(stderr, "Option -%c requires a number in [1, 1024].\n", optopt);
                    exit(1);
                }
                break;
            case 'f':
                (*fileName) = optarg;
                break;
            case '?':
                if (optopt == 'f')
                    fprintf (stderr, "Option -%c requires a valid fileName.\n", optopt);
                else if (optopt == 'p')
                    fprintf(stderr, "Option -%c requires a number in [1, 1024].\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                exit(1);
            default:
                abort ();
        }
}

int CountLines(FILE *fp, int lineCounter) {
    char* buffer = (char *) calloc(MAXLENGTH, sizeof(char));
    while (fgets(buffer, MAXLENGTH, fp) != NULL) {
        lineCounter++; // count lines(processes)
    }
    free(buffer);
    fclose(fp);
    return lineCounter;
}

input_node_ptr ReadFile(const char *fileName, FILE *fp) {// linked list to store items from the input file
    input_node_ptr input_list_head = NULL;

    // extract process info from the file
    // and store them into a linked list
    fp = fopen(fileName, "r");
    char* buffer = (char *) calloc(MAXLENGTH, sizeof(char));
    int timeArrived = 0, processId = 0, executionTime = 0, parallelisable = 0;
    int* start = (int *) malloc(sizeof(int));
    if (start == NULL) exit(1);
    int* end = (int *) malloc(sizeof(int));
    if (end == NULL) exit(1);
    while (fgets(buffer, MAXLENGTH, fp) != NULL) {
        if(buffer[0] == '\n') break;
        *start = -1;
        *end = *start;
        timeArrived = ExtractIntNumber(buffer, start, end);
        processId = ExtractIntNumber(buffer, start, end);
        executionTime = ExtractIntNumber(buffer, start, end);
        parallelisable = ExtractPN(buffer, start, end);
        // printf("Debug %d, %d, %d, %d\n", timeArrived, processId,executionTime, parallelisable);
        input_list_head = InputListInsert(input_list_head, timeArrived, processId,executionTime, parallelisable);
    }
    free(start);
    free(end);
    free(buffer);
    return input_list_head;
}

void SimRun(input_node_ptr input_list_head, int numMainProcess, int numCPU, int useOwnScheduler) {
    int globalTimer = 0;
    int totalDeltaTime = 0;
    int numRunnedProcesses = 0;
    float maxTimeOverhead = 0;
    float totalTimeOverhead = 0;

    input_node_ptr input_process_ptr;

    if (numCPU == 1) {
        input_list_head = SortInputListByPid(input_list_head);
        input_list_head = SortInputListByArrival(input_list_head);

        // init one CPU running Priority queue
        PQ_t pq = InitializePQ(numMainProcess, 0);
        process_t* minRemainTimeProcess_ptr = NULL;

        // if there are still items in the input_list
        while (input_list_head->next != NULL) {

            // try to get one process record from input list
            // if the one has not been insert in running q,
            // remains the previous value
            input_process_ptr = GetFromInputList(input_list_head);

            // when it is the time for the process arrive,
            // insert the one to the q
            // and update(clean) the input_list
            while (globalTimer == input_process_ptr->time_arrived) {

                // insert the one into the PQ in remaining time
                InsertPQ(createProcess(input_process_ptr->process_id,
                                       input_process_ptr->time_arrived,
                                       input_process_ptr->execution_time,
                                       input_process_ptr->parallelisable,
                                       0, 0)
                         , pq);


                // remove the running progress from input_list
                UpdateInputList(input_list_head);

                // check the PQ for proc with min remain time after insert new process
                process_t* newMinRaminTimeProcess = FindMinRemainTimeProcess(pq);

                // if there is a prev minRemainTime is in PQ, and new min process is not the prev one, prev should pause
                // This is the only place that can make running proc pause in CPU PQ
                if(minRemainTimeProcess_ptr != NULL &&
                   minRemainTimeProcess_ptr->pid != newMinRaminTimeProcess->pid) {
                    minRemainTimeProcess_ptr->isRunning = 0;
                }
                // pick the shortest one that will run in CPU running PQ.
                minRemainTimeProcess_ptr = newMinRaminTimeProcess;
                free(input_process_ptr);

                // Handling  the same time:
                // pick the next one in the input_list, in case of the same time input
                // if the input_list is empty, skip this loop
                if(input_list_head->next == NULL) {
                    break;
                }
                input_process_ptr = GetFromInputList(input_list_head);
            }

            // if there is no progress in the running queue
            if (IsEmptyPQ(pq)) {

                globalTimer++;
                continue;

            } else {
                // if there are progress in the running queue
                // run the most short remain time process, set run status to 1
                // and decrease remaining time by 1
                // minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                if(minRemainTimeProcess_ptr->isRunning == 0) {
                    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",
                           globalTimer,
                           minRemainTimeProcess_ptr->pid,
                           minRemainTimeProcess_ptr->remainingTime,
                           0);
                    minRemainTimeProcess_ptr->isRunning = 1;
                }

                minRemainTimeProcess_ptr->remainingTime -= 1;
            }

            globalTimer++;

            // check the shortest one, whether it is finished
            if (minRemainTimeProcess_ptr->remainingTime == 0) {
                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",
                       globalTimer,
                       minRemainTimeProcess_ptr->pid,
                       pq->size - 1);

                // collect statistic data
                int deltaTime = globalTimer - minRemainTimeProcess_ptr->arrivalTime;
                totalDeltaTime += deltaTime;
                float timeOverhead = (float)deltaTime / (minRemainTimeProcess_ptr->burstTime);
                if(timeOverhead > maxTimeOverhead) {
                    maxTimeOverhead = timeOverhead;
                }
                totalTimeOverhead += timeOverhead;
                numRunnedProcesses++;

                // free and delete the ended progress from CPU running pq
                free(DeleteMinRemainTimeProcess(pq));

                // pick one new shortest process from CPU running pq
                if(!IsEmptyPQ(pq)) {
                    minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                }
            }
        }

        // list is empty, clear the input_list dummy head
        free(input_list_head);

        // if there are no more processs arrive, but there are still something remaining in the CPU
        while (!IsEmptyPQ(pq)) {
            // pick the shortest one to run
            // minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
            if(minRemainTimeProcess_ptr->isRunning == 0) {
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",
                       globalTimer,
                       minRemainTimeProcess_ptr->pid,
                       minRemainTimeProcess_ptr->remainingTime,
                       0);
                minRemainTimeProcess_ptr->isRunning = 1;
            }
            minRemainTimeProcess_ptr->remainingTime -= 1;

            globalTimer++;
            // check the shortest one, whether it is finished
            if (minRemainTimeProcess_ptr->remainingTime == 0) {
                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",
                       globalTimer,
                       minRemainTimeProcess_ptr->pid,
                       pq->size - 1);

                // collect statistic data
                int deltaTime = globalTimer - minRemainTimeProcess_ptr->arrivalTime;
                totalDeltaTime += deltaTime;
                float timeOverhead = (float)deltaTime / (minRemainTimeProcess_ptr->burstTime);
                if(timeOverhead > maxTimeOverhead) {
                    maxTimeOverhead = timeOverhead;
                }
                totalTimeOverhead += timeOverhead;
                numRunnedProcesses++;

                // free and delete the ended progress
                free(DeleteMinRemainTimeProcess(pq));
                minRemainTimeProcess_ptr = NULL;

                // pick one new shortest process from running pq
                if(!IsEmptyPQ(pq)) {
                    minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                }
            }
        }

        // ending, free all cpu running PQ
        DestroyPQ(pq);

    } else if (numCPU >= 2) {

        input_list_head = SortInputListByPid(input_list_head);
        input_list_head = SortInputListByRemain(input_list_head);
        input_list_head = SortInputListByArrival(input_list_head);
        int procCounter = 0;

        PQ_t* cpuPQList = (PQ_t*) calloc(numCPU, sizeof (PQ_t));
        for(int i = 0; i < numCPU; i++) {
            cpuPQList[i] = InitializePQ(numMainProcess, i);
        }

        process_t** cpuMinProcess_ptr_list = (process_t**) calloc(numCPU, sizeof (process_t*));
        for(int i = 0; i < numCPU; i++) {
            cpuMinProcess_ptr_list[i] = NULL;
        }

        input_node_ptr parallelProcIndexList = NULL;

        // if there are still items in the input_list
        while (input_list_head->next != NULL) {

            // try to get one process record from input list
            // if the one has not been insert in running q,
            // remains the previous value
            input_process_ptr = GetFromInputList(input_list_head);

            // when it is the time for the process arrive,
            // insert the one to the q
            // and update(clean) the input_list
            while (globalTimer == input_process_ptr->time_arrived) {
                if(input_process_ptr->parallelisable == 0) {

                    // pick one cpu to add
                    PQ_t smallRemainTimeCpuPQ = cpuPQList[0];
                    int i = 0;
                    for(i = 0; i < numCPU; i++) {
                        if(CountTotalRemainingTime(smallRemainTimeCpuPQ) > CountTotalRemainingTime(cpuPQList[i])) {
                            smallRemainTimeCpuPQ = cpuPQList[i];
                        }
                    }

                    InsertPQ(createProcess(input_process_ptr->process_id,
                                           input_process_ptr->time_arrived,
                                           input_process_ptr->execution_time,
                                           input_process_ptr->parallelisable,
                                           smallRemainTimeCpuPQ->cpuId, 0),
                             smallRemainTimeCpuPQ);

                    // remove the running progress from input_list
                    UpdateInputList(input_list_head);

                    // check the PQ for proc with min remain time after insert new process
                    process_t* newMinRaminTimeProcess = FindMinRemainTimeProcess(smallRemainTimeCpuPQ);

                    // if there is a prev minRemainTime is in PQ, and new min process is not the prev one, prev should pause
                    // This is the only place that can make running proc pause in CPU PQ
                    if(cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId] != NULL &&
                            cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId]->pid != newMinRaminTimeProcess->pid) {
                        cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId]->isRunning = 0;
                    }
                    // pick the shortest one that will run in CPU running PQ.
                    cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId] = newMinRaminTimeProcess;
                    free(input_process_ptr);

                    // Handling  the same time:
                    // pick the next one in the input_list, in case of the same time input
                    // if the input_list is empty, skip this loop
                    if(input_list_head->next == NULL) {
                        break;
                    }
                    input_process_ptr = GetFromInputList(input_list_head);

                }
                    // if the process is parallel
                else if(input_process_ptr->parallelisable == 1) {

                    // if proc has NOT been added into indexList, add it
                    if(parallelProcIndexList == NULL ||
                       findIndexByPid(parallelProcIndexList, input_process_ptr->process_id) == NULL) {
                        // decide num subProcesses
                        int k = 1;

                        for(k = 1; k < numCPU; k++) {
                            if(((double)input_process_ptr->execution_time / k) < 1) {
                                k = k - 1;
                                break;
                            }
                        }
                        // when k == numCPU
                        if(((double)input_process_ptr->execution_time / k) < 1) {
                            k = k - 1;
                        }

                        // insert proc to para index list
                        parallelProcIndexList = parallelIndexInsert(parallelProcIndexList,
                                            input_process_ptr->time_arrived,
                                            input_process_ptr->process_id,
                                            input_process_ptr->execution_time, k);

                        // assign sub proc to different CPU PQ
                        for(int subProcNo = 0; subProcNo < k; subProcNo++) {

                            // pick one cpu to add sub processes
                            PQ_t smallRemainTimeCpuPQ = cpuPQList[0];
                            int i = 0;
                            for(i = 0; i < numCPU; i++) {
                                if(CountTotalRemainingTime(smallRemainTimeCpuPQ) > CountTotalRemainingTime(cpuPQList[i])) {
                                    smallRemainTimeCpuPQ = cpuPQList[i];
                                }
                            }

                            InsertPQ(createProcess(input_process_ptr->process_id,
                                                   input_process_ptr->time_arrived,
                                                   (int)(ceil((double)(input_process_ptr->execution_time)/k) + 1),
                                                   input_process_ptr->parallelisable,
                                                   smallRemainTimeCpuPQ->cpuId, subProcNo),
                                     smallRemainTimeCpuPQ);

                            // check the PQ for proc with min remain time after insert new process
                            process_t* newMinRaminTimeProcess = FindMinRemainTimeProcess(smallRemainTimeCpuPQ);

                            // if there is a prev minRemainTime is in PQ, and new min process is not the prev one, prev should pause
                            // This is the only place that can make running proc pause in CPU PQ
                            if(cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId] != NULL &&
                               cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId]->pid != newMinRaminTimeProcess->pid) {
                                cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId]->isRunning = 0;
                            }
                            // pick the shortest one that will run in CPU running PQ.
                            cpuMinProcess_ptr_list[smallRemainTimeCpuPQ->cpuId] = newMinRaminTimeProcess;
                        }



                        // remove the running progress from input_list
                        UpdateInputList(input_list_head);

                        free(input_process_ptr);

                        // Handling  the same time:
                        // pick the next one in the input_list, in case of the same time input
                        // if the input_list is empty, skip this loop
                        if(input_list_head->next == NULL) {
                            break;
                        }
                        input_process_ptr = GetFromInputList(input_list_head);
                    }
                }
            }

            // if there is no progress in every CPU running queue
            if (IsAllCpuPQEmpty(cpuPQList, numCPU)) {

                globalTimer++;
                continue;
            } else {
                // if there are progress in the running queue
                // run the most short remain time process, set run status to 1
                // and decrease remaining time by 1
                // minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                for(int i = 0 ; i < numCPU; i++) {

                    // if the min process ptr points to NULL, find next cpu's
                    if(cpuMinProcess_ptr_list[i] == NULL) {
                        // printf("the min process ptr points to NULL\n");
                        continue;
                    }

                    if(cpuMinProcess_ptr_list[i]->isRunning == 0) {
                        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",
                               globalTimer,
                               cpuMinProcess_ptr_list[i]->pid,
                               cpuMinProcess_ptr_list[i]->remainingTime,
                               cpuMinProcess_ptr_list[i]->cpuId);
                        cpuMinProcess_ptr_list[i]->isRunning = 1;
                    }

                    cpuMinProcess_ptr_list[i]->remainingTime -= 1;
                }
            }

            globalTimer++;

            for(int i = 0 ; i < numCPU; i++) {
                // if the min process ptr points to NULL, find next cpu's
                if(cpuMinProcess_ptr_list[i] == NULL) {
                    // printf("the min process ptr points to NULL\n");
                    continue;
                }

                // check the shortest one, whether it is finished
                procCounter = CountAllProcesses(cpuPQList, numCPU);
                if (cpuMinProcess_ptr_list[i]->remainingTime == 0) {
                    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",
                           globalTimer,
                           cpuMinProcess_ptr_list[i]->pid,
                           procCounter);

                    // collect statistic data
                    int deltaTime = globalTimer - cpuMinProcess_ptr_list[i]->arrivalTime;
                    totalDeltaTime += deltaTime;
                    float timeOverhead = (float)deltaTime / (cpuMinProcess_ptr_list[i]->burstTime);
                    if(timeOverhead > maxTimeOverhead) {
                        maxTimeOverhead = timeOverhead;
                    }
                    totalTimeOverhead += timeOverhead;
                    numRunnedProcesses++;

                    // free and delete the ended progress from CPU running pq
                    free(DeleteMinRemainTimeProcess(cpuPQList[i]));
                    cpuMinProcess_ptr_list[i] = NULL;

                    // pick one new shortest process from CPU running pq
                    if(!IsEmptyPQ(cpuPQList[i])) {
                        cpuMinProcess_ptr_list[i] = FindMinRemainTimeProcess(cpuPQList[i]);
                    }
                }

            }
        }

        // list is empty, clear the input_list dummy head
        free(input_list_head);

        // if there are no more processs arrive, but there are still something remaining in the CPU
        // if all cpu don't have procs, done!
        while(!IsAllCpuPQEmpty(cpuPQList, numCPU)) {

            for (int i = 0; i < numCPU; i++) {

                // don't need to worry about free cpu
                if (!IsEmptyPQ(cpuPQList[i])) {
                    // pick the shortest one to run
                    // minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                    if (cpuMinProcess_ptr_list[i]->isRunning == 0) {
                        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",
                               globalTimer,
                               cpuMinProcess_ptr_list[i]->pid,
                               cpuMinProcess_ptr_list[i]->remainingTime,
                               cpuMinProcess_ptr_list[i]->cpuId);
                        cpuMinProcess_ptr_list[i]->isRunning = 1;
                    }
                    cpuMinProcess_ptr_list[i]->remainingTime -= 1;
                }
            }

            globalTimer++;

            for (int i = 0; i < numCPU; i++) {
                // don't need to worry about free cpu
                if(!IsEmptyPQ(cpuPQList[i])) {
                    // check the shortest one, whether it is finished
                    procCounter = CountAllProcesses(cpuPQList, numCPU);
                    if (cpuMinProcess_ptr_list[i]->remainingTime == 0) {
                        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",
                               globalTimer,
                               cpuMinProcess_ptr_list[i]->pid,
                               procCounter);

                        // collect statistic data
                        int deltaTime = globalTimer - cpuMinProcess_ptr_list[i]->arrivalTime;
                        totalDeltaTime += deltaTime;
                        float timeOverhead = (float)deltaTime / (cpuMinProcess_ptr_list[i]->burstTime);
                        if(timeOverhead > maxTimeOverhead) {
                            maxTimeOverhead = timeOverhead;
                        }
                        totalTimeOverhead += timeOverhead;
                        numRunnedProcesses++;

                        // free and delete the ended progress
                        // free and delete the ended progress from CPU running pq
                        free(DeleteMinRemainTimeProcess(cpuPQList[i]));
                        cpuMinProcess_ptr_list[i] = NULL;

                        // pick one new shortest process from running pq
                        if(!IsEmptyPQ(cpuPQList[i])) {
                            cpuMinProcess_ptr_list[i] = FindMinRemainTimeProcess(cpuPQList[i]);
                        }
                    }
                }
            }

        }


        // ending, free all cpu running PQ
        for(int i = 0; i < numCPU; i++) {
            DestroyPQ(cpuPQList[i]);
        }
        free(cpuPQList);
        free(cpuMinProcess_ptr_list);
    }

    printf("Turnaround time %d\n", RoundToInt((float)totalDeltaTime / numRunnedProcesses));
    printf("Time overhead %g %g\n", RoundToTwoDigitFloat(maxTimeOverhead), RoundToTwoDigitFloat(totalTimeOverhead / numRunnedProcesses));
    printf("Makespan %d\n", globalTimer);
}

int RoundToInt(float f)
{
    int dint=(int) f;

    if(f >= dint + 0.5)
    {
        return dint+1;
    }
    else
    {
        return dint;
    }
}

float RoundToTwoDigitFloat(float f) {
    return ((int)(f * 100 + 0.5)/ 100.0);
    // return f;
}