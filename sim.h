//
// Created by Shaotien Lee on 2021/3/23.
//

#ifndef COMP30023_2021_PROJECT_1_SIM_H
#define COMP30023_2021_PROJECT_1_SIM_H

void SimRun(input_node_ptr input_list_head, int numMainProcess, int numCPU, int useOwnScheduler);
input_node_ptr ReadFile(const char *fileName, FILE *fp);
unsigned int RoundToInt(double f);
double RoundToTwoDigit(double f);
int CountLines(FILE *fp, int lineCounter);
void
GetArgs(int argc, char **argv, char **charPart, int argChar, int *useOwnScheduler, int *numProcessors, char **fileName);

#endif //COMP30023_2021_PROJECT_1_SIM_H
