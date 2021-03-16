#include <stdio.h>
#include <stdlib.h>
#include "process.h"

#include <ctype.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char** argv) {

    int useOwnScheduler = 0;
    char* charPart = NULL;
    int numProcessors = 0;
    char* fileName = NULL;
    int index;
    int c;

    opterr = 0;

    // handle args
    while ((c = getopt (argc, argv, "cp:f:")) != -1)
        switch (c)
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

    printf ("useOwnScheduler = %d, numProcessors= %d, fileName = %s\n",
            useOwnScheduler, numProcessors, fileName);

    for (index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);

    process_t* process1 = createProcess(1, 1, 2, 0);
    process_t* process2 = createProcess(2, 2, 4, 0);
    process_t* process3 = createProcess(3, 3, 1, 0);
    process_t* process4 = createProcess(4, 4, 3, 0);

    PQ_t pq = InitializePQ(4);
    InsertPQ(process1, pq);
    InsertPQ(process2, pq);
    InsertPQ(process3, pq);
    InsertPQ(process4, pq);

    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);

    InsertPQ(process1, pq);
    InsertPQ(process2, pq);

    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);
    printf("%d\n", (DeleteMinRemainTimeProcess(pq))->remainingTime);

    DestroyPQ(pq);
    free(process1);
    free(process2);
    free(process3);
    free(process4);

    return 0;
}
