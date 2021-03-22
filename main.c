#include <stdio.h>
#include <unistd.h>

#include "linkedlist.h"
#include "sim.h"

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

    /* Open the input file with the given filename for reading */
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

