#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
    int current_queue;
    bool waiting;
    bool finished;
    bool executing;

    // Statistics
    int exec_count;
    int int_count;
    int turnaround;
    int response;
    int waiting_time;

};

// Process Queue
typedef struct queue {
    Process* head;
    Process* tail;
    int length;
    int priority;
} Queue;

// Process functions
Process* create_process(char** process_info);
Process** load_processes(InputFile* file);
void write_output(Process** processes, int quantity, char* filename);
void destroy_processes(Process** processes, int quantity);

// Queue functions
Queue** create_queues(int n);
void push(Queue* queue, Process* process);
void merge(Queue* from, Queue* to);
Process* pop(Queue* queue);
void destroy_queues(Queue** queues, int quantity);
