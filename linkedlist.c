//
// Created by Xiaotian Li on 2021/3/16.
//
#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>

input_node_ptr InputListInsert(input_node_ptr head, int arrivalTime, int pid, int executionTime, int isParallel){

    if(head == NULL) {
        input_node_ptr dummyHead = (input_node_ptr) calloc(1, sizeof(input_node_t));
        dummyHead->next = (input_node_ptr) calloc(1, sizeof(input_node_t));
        head = dummyHead->next;
        head->next = NULL;
        head->parallelisable = isParallel;
        head->execution_time = executionTime;
        head->time_arrived = arrivalTime;
        head->process_id = pid;
        head->remainNumSubs = 0;
        head->numSubs = 0;
        return dummyHead;
    }

    input_node_ptr dummy = head->next;
    while(dummy->next != NULL) {
        dummy = dummy->next;
    }

    input_node_ptr newNode = (input_node_ptr) calloc(1, sizeof(input_node_t));
    dummy->next = newNode;
    newNode->next = NULL;
    newNode->parallelisable = isParallel;
    newNode->execution_time = executionTime;
    newNode->time_arrived = arrivalTime;
    newNode->process_id = pid;
    newNode->remainNumSubs = 0;
    newNode->numSubs = 0;

    return head;
}
///0->1->2->3->4->5->null

input_node_ptr GetFromInputList(input_node_ptr dummyHead) {

    if(dummyHead == NULL || dummyHead->next == NULL){
        printf("Empty linked list\n");
        exit(-1);
    }

    input_node_ptr temp = dummyHead->next;

//    dummyHead->next = temp->next;
    return temp;
}

void UpdateInputList(input_node_ptr dummyHead) {

    if(dummyHead == NULL || dummyHead->next == NULL){
        printf("Empty linked list\n");
        exit(-1);
    }

    input_node_ptr temp = dummyHead->next;
    dummyHead->next = temp->next;
}

input_node_ptr parallelIndexInsert(input_node_ptr head, int arrivalTime, int pid, int executionTime, int numSUbs) {
    if(head == NULL) {
        input_node_ptr dummyHead = (input_node_ptr) calloc(1, sizeof(input_node_t));
        dummyHead->next = (input_node_ptr) calloc(1, sizeof(input_node_t));
        head = dummyHead->next;
        head->next = NULL;
        head->parallelisable = 1;
        head->execution_time = executionTime;
        head->time_arrived = arrivalTime;
        head->process_id = pid;
        head->numSubs = numSUbs;
        head->remainNumSubs = numSUbs;
        return dummyHead;
    }

    input_node_ptr dummy = head->next;
    while(dummy->next != NULL) {
        dummy = dummy->next;
    }

    input_node_ptr newNode = (input_node_ptr) calloc(1, sizeof(input_node_t));
    dummy->next = newNode;
    newNode->next = NULL;
    newNode->parallelisable = 1;
    newNode->execution_time = executionTime;
    newNode->time_arrived = arrivalTime;
    newNode->process_id = pid;
    head->numSubs = numSUbs;
    head->remainNumSubs = numSUbs;

    return head;
}

void removeIndexByPid(input_node_ptr dummyHead, int pid) {
    if(dummyHead == NULL || dummyHead->next == NULL){
        printf("Empty index list\n");
        exit(-1);
    }

    input_node_ptr ptr = dummyHead->next;
    input_node_ptr slow_ptr = dummyHead;

    while ( ptr != NULL && ptr->process_id != pid) {
        ptr = ptr->next;
        slow_ptr = slow_ptr->next;
    }

    if(ptr == NULL) {
        printf("pid not found in parallel Index list!");
        return;
    }

    slow_ptr->next = ptr->next;
    free(ptr);
}

input_node_ptr findIndexByPid(input_node_ptr dummyHead, int pid) {
    if(dummyHead == NULL || dummyHead->next == NULL){
        printf("Empty index list\n");
        exit(-1);
    }

    input_node_ptr ptr = dummyHead->next;

    while ( ptr != NULL && ptr->process_id != pid) {
        ptr = ptr->next;
    }

    if(ptr == NULL) {
        return NULL;
    }

    return ptr;
}

input_node_ptr SortInputListByRemain(input_node_ptr L)
{
    input_node_ptr p,q,small;
//    input_node_ptr temp = (input_node_ptr) calloc(1, sizeof (input_node_ptr));
//    if(temp == NULL) {
//        exit(1);
//    }

    // use these to handle memory read/write bug
    int time_arrived = 0;
    int process_id = 0;
    int execution_time = 0;
    int parallelisable = 0;
    int numSubs = 0;
    int remainNumSubs = 0;

    for(p = L->next; p->next != NULL; p = p->next)
    {
        small = p;
        for(q = p->next; q; q = q->next)
        {
            if(q->execution_time < small->execution_time)
            {
                small = q;
            }
        }

        if(small != p)
        {
            execution_time = p->execution_time;
            parallelisable = p->parallelisable;
            time_arrived = p->time_arrived;
            process_id = p->process_id;
            numSubs = p->numSubs;
            remainNumSubs = p->remainNumSubs;

            p->execution_time = small->execution_time;
            p->parallelisable = small->parallelisable;
            p->time_arrived = small->time_arrived;
            p->process_id = small->process_id;
            p->numSubs = small->numSubs;
            p->remainNumSubs = small->remainNumSubs;

            small->execution_time = execution_time;
            small->parallelisable = parallelisable;
            small->time_arrived = time_arrived;
            small->process_id = process_id;
            small->numSubs = numSubs;
            small->remainNumSubs = remainNumSubs;
        }
    }

    // free(temp);
    return L;
}

input_node_ptr SortInputListByPid(input_node_ptr L)
{
    input_node_ptr p,q,small;
//    input_node_ptr temp = (input_node_ptr) calloc(1, sizeof (input_node_ptr));
//    if(temp == NULL) {
//        exit(1);
//    }

    // use these to handle memory read/write bug
    int time_arrived = 0;
    int process_id = 0;
    int execution_time = 0;
    int parallelisable = 0;
    int numSubs = 0;
    int remainNumSubs = 0;

    for(p = L->next; p->next != NULL; p = p->next)
    {
        small = p;
        for(q = p->next; q; q = q->next)
        {
            if(q->process_id < small->process_id)
            {
                small = q;
            }
        }

        if(small != p)
        {
//            // Leak
//            temp->execution_time = p->execution_time;
//            // Leak
//            temp->parallelisable = p->parallelisable;
//            temp->time_arrived = p->time_arrived;
//            temp->process_id = p->process_id;
//            // Leak
//            temp->numSubs = p->numSubs;
//            // Leak
//            temp->remainNumSubs = p->remainNumSubs;

            execution_time = p->execution_time;
            parallelisable = p->parallelisable;
            time_arrived = p->time_arrived;
            process_id = p->process_id;
            numSubs = p->numSubs;
            remainNumSubs = p->remainNumSubs;

            p->execution_time = small->execution_time;
            p->parallelisable = small->parallelisable;
            p->time_arrived = small->time_arrived;
            p->process_id = small->process_id;
            p->numSubs = small->numSubs;
            p->remainNumSubs = small->remainNumSubs;

//            small->execution_time = temp->execution_time;
//            //leak
//            small->parallelisable = temp->parallelisable;
//            small->time_arrived = temp->time_arrived;
//            //leak
//            small->process_id = temp->process_id;
//            // Leak
//            small->numSubs = temp->numSubs;
//            // Leak
//            small->remainNumSubs = temp->remainNumSubs;

            small->execution_time = execution_time;
            small->parallelisable = parallelisable;
            small->time_arrived = time_arrived;
            small->process_id = process_id;
            small->numSubs = numSubs;
            small->remainNumSubs = remainNumSubs;
        }
    }

    //free(temp);
    return L;
}

input_node_ptr SortInputListByArrival(input_node_ptr L) {
    input_node_ptr p,q,small;
//    input_node_ptr temp = (input_node_ptr) calloc(1, sizeof (input_node_ptr));
//    if(temp == NULL) {
//        exit(1);
//    }

    // use these to handle memory write error
    int time_arrived = 0;
    int process_id = 0;
    int execution_time = 0;
    int parallelisable = 0;
    int numSubs = 0;
    int remainNumSubs = 0;

    for(p = L->next; p->next != NULL; p = p->next)
    {
        small = p;
        for(q = p->next; q; q = q->next)
        {
            if(q->time_arrived < small->time_arrived)
            {
                small = q;
            }
        }

        if(small != p)
        {
            execution_time = p->execution_time;
            parallelisable = p->parallelisable;
            time_arrived = p->time_arrived;
            process_id = p->process_id;
            numSubs = p->numSubs;
            remainNumSubs = p->remainNumSubs;

            p->execution_time = small->execution_time;
            p->parallelisable = small->parallelisable;
            p->time_arrived = small->time_arrived;
            p->process_id = small->process_id;
            p->numSubs = small->numSubs;
            p->remainNumSubs = small->remainNumSubs;

            small->execution_time = execution_time;
            small->parallelisable = parallelisable;
            small->time_arrived = time_arrived;
            small->process_id = process_id;
            small->numSubs = numSubs;
            small->remainNumSubs = remainNumSubs;
        }
    }

    //free(temp);
    return L;
}