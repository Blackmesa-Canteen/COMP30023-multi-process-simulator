//
// Created by xiaotian on 2021/3/16.
//

#ifndef COMP30023_2021_PROJECT_1_LINKEDLIST_H
#define COMP30023_2021_PROJECT_1_LINKEDLIST_H

typedef struct node {

    unsigned int time_arrived;
    unsigned int process_id;
    unsigned int execution_time;
    int parallelisable;
    int numSubs;
    int remainNumSubs;

    struct node* next;
} input_node_t;

typedef input_node_t* input_node_ptr;

input_node_ptr InputListInsert(input_node_ptr head, unsigned int arrivalTime, unsigned int pid, unsigned int executionTime, int isParallel);
input_node_ptr parallelIndexInsert(input_node_ptr head, unsigned int arrivalTime, unsigned int pid, unsigned int executionTime, int numSUbs);
input_node_ptr destroyLinkList(input_node_ptr dummyHead);
input_node_ptr GetFromInputList(input_node_ptr dummyHead);
void UpdateInputList(input_node_ptr dummyHead);

void removeIndexByPid(input_node_ptr dummyHead, unsigned int pid);
input_node_ptr findIndexByPid(input_node_ptr dummyHead, unsigned int pid);
input_node_ptr SortInputListByRemain(input_node_ptr L);
input_node_ptr SortInputListByPid(input_node_ptr L);
input_node_ptr SortInputListByArrival(input_node_ptr L);

#endif //COMP30023_2021_PROJECT_1_LINKEDLIST_H
