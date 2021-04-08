#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include "../file_manager/manager.h"
#pragma once


// Create a new process
int create_process(InputFile* file, int index);

// Handle worker process workflow
void handle_worker(InputFile* file, int index);

// Handle manager process workflow
void handle_manager(InputFile* file, int index);

// TODO: Write Output functions
