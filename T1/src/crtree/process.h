#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../file_manager/manager.h"
#pragma once


typedef struct Process {
    int pid;
    int* children;

} Process;


/* TODO:
- Process struct
- Worker Process
- Manager Process (Root & Normal)
- Write Output
*/


/* Process init
- id_type: id for each process type (worker, manager, root)
- children: array of pointers to each child process
- forks, then returns id to parent
*/

// Process destroy

int create_process(InputFile* file, int index);

void handle_worker(char* codename, char** args);

void handle_manager(InputFile* file, int timeout, int child_count, int* children);

void write_output(Process* process);