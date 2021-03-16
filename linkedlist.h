//
// Created by xiaotian on 2021/3/16.
//

#ifndef COMP30023_2021_PROJECT_1_LINKEDLIST_H
#define COMP30023_2021_PROJECT_1_LINKEDLIST_H

typedef struct node {

    int time_arrived;
    int process_id;
    int execution_time;
    int parallelisable;

    struct node* next;
} input_node_t;

typedef input_node_t* input_node_ptr;

input_node_ptr InputListInsert(input_node_ptr head, int arrivalTime, int pid, int executionTime, int isParallel);
input_node_ptr GetFromInputList(input_node_ptr dummyHead);
void UpdateInputList(input_node_ptr dummyHead);

#endif //COMP30023_2021_PROJECT_1_LINKEDLIST_H
