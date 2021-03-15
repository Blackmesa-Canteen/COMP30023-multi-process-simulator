#include <stdio.h>
#include <stdlib.h>
#include "process.h"

int main() {
    printf("Hello, World!\n");
    process_t* process1 = createProcess(1, 1, 2, 0);
    process_t* process2 = createProcess(2, 2, 4, 0);
    process_t* process3 = createProcess(3, 3, 1, 0);
    process_t* process4 = createProcess(4, 4, 3, 0);

    PQ_t pq = InitializePQ(4);
    InsertPQ(process1, pq);
    InsertPQ(process2, pq);
    InsertPQ(process3, pq);
    InsertPQ(process4, pq);

    process_t* buffer;
    printf("%d", (buffer = DeleteMinRemainTimeProcess(pq))->remainingTime);
    free(buffer);
    printf("%d", (buffer = DeleteMinRemainTimeProcess(pq))->remainingTime);
    free(buffer);
    printf("%d", (buffer = DeleteMinRemainTimeProcess(pq))->remainingTime);
    free(buffer);
    printf("%d", (buffer = DeleteMinRemainTimeProcess(pq))->remainingTime);
    free(buffer);

    DestroyPQ(pq);

    return 0;
}
