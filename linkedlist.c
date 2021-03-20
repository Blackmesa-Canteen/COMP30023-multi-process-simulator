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
        head->execution_time = executionTime;
        head->time_arrived = arrivalTime;
        head->process_id = pid;
        head->numSubs = numSUbs;
        return dummyHead;
    }

    input_node_ptr dummy = head->next;
    while(dummy->next != NULL) {
        dummy = dummy->next;
    }

    input_node_ptr newNode = (input_node_ptr) calloc(1, sizeof(input_node_t));
    dummy->next = newNode;
    newNode->next = NULL;
    newNode->execution_time = executionTime;
    newNode->time_arrived = arrivalTime;
    newNode->process_id = pid;
    head->numSubs = numSUbs;

    return head;
}

void removeIndexByPid(input_node_ptr dummyHead, int pid) {
    if(dummyHead == NULL || dummyHead->next == NULL){
        printf("Empty index list\n");
        exit(-1);
    }

    input_node_ptr ptr = dummyHead->next;
    input_node_ptr slow_ptr = dummyHead;

    while (ptr->process_id != pid && ptr != NULL) {
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

    while (ptr->process_id != pid && ptr != NULL) {
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
    input_node_ptr temp = (input_node_ptr) calloc(1, sizeof (input_node_ptr));

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
            temp->execution_time = p->execution_time;
            temp->parallelisable = p->parallelisable;
            temp->time_arrived = p->time_arrived;
            temp->process_id = p->process_id;
            temp->numSubs = p->numSubs;

            p->execution_time = small->execution_time;
            p->parallelisable = small->parallelisable;
            p->time_arrived = small->time_arrived;
            p->process_id = small->process_id;
            p->numSubs = small->numSubs;

            small->execution_time = temp->execution_time;
            small->parallelisable = temp->parallelisable;
            small->time_arrived = temp->time_arrived;
            small->process_id = temp->process_id;
            small->numSubs = temp->numSubs;
        }
    }

    free(temp);
    return L;
}

input_node_ptr SortInputListByPid(input_node_ptr L)
{
    input_node_ptr p,q,small;
    input_node_ptr temp = (input_node_ptr) calloc(1, sizeof (input_node_ptr));

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
            temp->execution_time = p->execution_time;
            temp->parallelisable = p->parallelisable;
            temp->time_arrived = p->time_arrived;
            temp->process_id = p->process_id;
            temp->numSubs = p->numSubs;

            p->execution_time = small->execution_time;
            p->parallelisable = small->parallelisable;
            p->time_arrived = small->time_arrived;
            p->process_id = small->process_id;
            p->numSubs = small->numSubs;

            small->execution_time = temp->execution_time;
            small->parallelisable = temp->parallelisable;
            small->time_arrived = temp->time_arrived;
            small->process_id = temp->process_id;
            small->numSubs = temp->numSubs;
        }
    }

    free(temp);
    return L;
}

input_node_ptr SortInputListByArrival(input_node_ptr L) {
    input_node_ptr p,q,small;
    input_node_ptr temp = (input_node_ptr) calloc(1, sizeof (input_node_ptr));

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
            temp->execution_time = p->execution_time;
            temp->parallelisable = p->parallelisable;
            temp->time_arrived = p->time_arrived;
            temp->process_id = p->process_id;
            temp->numSubs = p->numSubs;

            p->execution_time = small->execution_time;
            p->parallelisable = small->parallelisable;
            p->time_arrived = small->time_arrived;
            p->process_id = small->process_id;
            p->numSubs = small->numSubs;

            small->execution_time = temp->execution_time;
            small->parallelisable = temp->parallelisable;
            small->time_arrived = temp->time_arrived;
            small->process_id = temp->process_id;
            small->numSubs = temp->numSubs;
        }
    }

    free(temp);
    return L;
}