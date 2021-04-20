#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../file_manager/manager.h"
#pragma once

// Process
struct process;
typedef struct process Process;
struct process {

    // Linked list
    Process* next;

    // Input data
    char* name;
    int pid;
    int start_time;
    int cycles;
    int wait;
    int waiting_delay;
    int initial_wait;

    // Simulation
    int quantum;

    // Statistics
    int exec_count;

};

// Process Queue
typedef struct queue {
    Process* head;
    Process* tail;
    int length;
    int priority;
} Queue;

// Process functions
Process* create_process(char* process_info[6]);
Process** load_processes(InputFile* file);
void destroy_processes(Process** processes, int quantity);

// Queue functions
Queue** create_queues(int n);
void push(Queue* queue, Process* process);
Process* pop(Queue* queue);
void destroy_queues(Queue** queues, int quantity);
