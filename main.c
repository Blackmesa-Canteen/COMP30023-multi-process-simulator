#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "readFile.h"
#include "linkedlist.h"

#include <ctype.h>
#include <unistd.h>
#include <string.h>

#define MAXLENGTH 513

void SimRun(input_node_ptr input_list_head, int numMainProcess, int numCPU, int useOwnScheduler);

int main(int argc, char** argv) {

    int useOwnScheduler = 0;
    char* charPart = NULL;
    int numProcessors = 1;
    char* fileName = NULL;
    int index;
    int argChar;
    opterr = 0;

    // handle args
    while ((argChar = getopt (argc, argv, "cp:f:")) != -1)
        switch (argChar)
        {
            case 'c':
                useOwnScheduler = 1;
                break;
            case 'p':
                numProcessors =  (int)strtol(optarg, &charPart, 10);
                if(strlen(charPart) != 0 || (numProcessors <= 0 || numProcessors > 1024)) {
                    fprintf(stderr, "Option -%c requires a number in [1, 1024].\n", optopt);
                    return 1;
                }
                break;
            case 'f':
                fileName = optarg;
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
                return 1;
            default:
                abort ();
        }

    /* Open the input file with the given filename for reading
     * for file : time-arrived, process-id, execution-time, parallelisable
     */
    FILE *fp = fopen(fileName, "r");
    if (!fp) {
        printf("can't opening file '%s'\n", fileName);
        return -1;
    }

    char* buffer = (char *) calloc(MAXLENGTH, sizeof(char));
    int timeArrived = 0, processId = 0, executionTime = 0, parallelisable = 0;

    int lineCounter = 0;
    while (fgets(buffer, MAXLENGTH, fp) != NULL) {
        lineCounter++; // count lines(processes)
    }
    fclose(fp);

    // PQ_t pq = InitializePQ(lineCounter);

    fp = fopen(fileName, "r");
    int* start = (int *) malloc(sizeof(int));
    if (start == NULL) exit(1);
    int* end = (int *) malloc(sizeof(int));
    if (end == NULL) exit(1);

    // linked list to store items from the input file
    input_node_ptr input_list_head = NULL;

    // extract process info from the file
    // and store them into a linked list
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

        // printf("-----------\n");
    }
    free(start);
    free(end);
    free(buffer);

    SimRun(input_list_head, lineCounter, numProcessors, useOwnScheduler);


//    process_t* process1 = createProcess(1, 1, 2, 0);
//    process_t* process2 = createProcess(2, 2, 4, 0);
//    process_t* process3 = createProcess(3, 3, 1, 0);
//    process_t* process4 = createProcess(4, 4, 3, 0);
//
//    InsertPQ(process1, pq);
//    InsertPQ(process2, pq);
//    InsertPQ(process3, pq);
//    InsertPQ(process4, pq);
//
//    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
//    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
//    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
//    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
//
//    InsertPQ(process1, pq);
//    InsertPQ(process2, pq);
//
//    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
//    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);

//    DestroyPQ(pq);
//    free(process1);
//    free(process2);
//    free(process3);
//    free(process4);

    return 0;
}

void SimRun(input_node_ptr input_list_head, int numMainProcess, int numCPU, int useOwnScheduler) {
    int globalTimer = 0;
    input_node_ptr input_process_ptr;
    process_t* minRemainTimeProcess_ptr;

    if (numCPU == 1) {

        PQ_t pq = InitializePQ(numMainProcess, 0);

        // if there are still items in the input_list
        while (input_list_head->next != NULL) {

            // try to get one process record from input list
            // if the one has not been insert in running q,
            // remains the previous value
            input_process_ptr = GetFromInputList(input_list_head);

            // when it is the time for the process arrive,
            // insert the one to the q
            // and update(clean) the input_list
            if(globalTimer >= input_process_ptr->time_arrived) {

                // insert the one into the PQ in remaining time
                InsertPQ(createProcess(input_process_ptr->process_id,
                                       input_process_ptr->time_arrived,
                                       input_process_ptr->execution_time,
                                       input_process_ptr->parallelisable)
                         , pq);


                // remove the running progress from input_list
                UpdateInputList(input_list_head);
                // pick the shortest one when a new process runs from running PQ.
                minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                free(input_process_ptr);
            }

            // if there is no progress in the running queue
            if (IsEmptyPQ(pq)) {

                globalTimer++;
                continue;

            } else {
                // if there are progress in the running queue
                // run the most short remain time process, and decrease remaining time by 1
                // minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",
                       globalTimer,
                       minRemainTimeProcess_ptr->pid,
                       minRemainTimeProcess_ptr->remainingTime,
                       0);
                minRemainTimeProcess_ptr->remainingTime -= 1;
            }

            globalTimer++;
            // check the shortest one, whether it is finished
            if (minRemainTimeProcess_ptr->remainingTime == 0) {
                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",
                       globalTimer,
                       minRemainTimeProcess_ptr->pid,
                       pq->size - 1);
                // free and delete the ended progress
                free(DeleteMinRemainTimeProcess(pq));

                // pick one new shortest process from running pq
                if(!IsEmptyPQ(pq)) {
                    minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                }
            }
        }

        // list is empty, clear the input_list dummy head
        free(input_list_head);

        // if the list is empty, but there are still something running
        while (!IsEmptyPQ(pq)) {

            // pick the shortest one to run
            // minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
            printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",
                   globalTimer,
                   minRemainTimeProcess_ptr->pid,
                   minRemainTimeProcess_ptr->remainingTime,
                   0);
            minRemainTimeProcess_ptr->remainingTime -= 1;

            globalTimer++;
            // check the shortest one, whether it is finished
            if (minRemainTimeProcess_ptr->remainingTime == 0) {
                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",
                       globalTimer,
                       minRemainTimeProcess_ptr->pid,
                       pq->size - 1);
                // free and delete the ended progress
                free(DeleteMinRemainTimeProcess(pq));

                // pick one new shortest process from running pq
                if(!IsEmptyPQ(pq)) {
                    minRemainTimeProcess_ptr = FindMinRemainTimeProcess(pq);
                }
            }
        }

        free(pq);
    }

}
