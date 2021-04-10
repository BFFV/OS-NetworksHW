#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "../file_manager/manager.h"
#pragma once


// Create a new process
int create_process(InputFile* file, int index);

// Handle worker process workflow
void handle_worker(InputFile* file, int index);

// Handle manager process workflow
void handle_manager(InputFile* file, int index);

// SIGABRT handler for worker process
void abort_worker(int signum);

// SIGABRT handler for manager process
void abort_manager(int signum);

// SIGINT handler for root process
void interrupt_root(int sigmun);
