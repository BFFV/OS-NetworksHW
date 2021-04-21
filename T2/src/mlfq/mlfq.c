#include "mlfq.h"

// Create new process
Process* create_process(char* process_info[6]) {
    Process* new_process = malloc(sizeof(Process));

    // Input data
    new_process->name = calloc(33, sizeof(char));
    new_process->name = process_info[0];
    new_process->pid = atoi(process_info[1]);
    new_process->start_time = atoi(process_info[2]);
    new_process->cycles = atoi(process_info[3]);
    new_process->wait = atoi(process_info[4]);
    new_process->initial_wait = atoi(process_info[4]);
    new_process->waiting_delay = atoi(process_info[5]);

    // Simulation
    new_process->quantum = 0;
    new_process->waiting = false;
    new_process->current_queue = 0;
    new_process->finished = false;
    new_process->executing = false;

    // Statistics
    new_process->exec_count = 0;
    new_process->int_count = 0;
    new_process->turnaround = 0;
    new_process->response = -1;
    new_process->waiting_time = 0;

    return new_process;
}

// Load process data from file
Process** load_processes(InputFile* file) {
    Process** process_array = malloc(file->len * sizeof(Process*));
    int index = 0;
    int current_time = 0;
    while (index < file->len) {
        for (int k = 0; k < file->len; k++) {
            if (atoi(file->lines[k][2]) == current_time) {
                process_array[index] = create_process(file->lines[k]);
                index++;
            }
        }
        current_time++;
    }
    return process_array;
}

// Write output file
void write_output(Process** processes, int quantity, char* filename) {
    FILE* file = fopen(strcat(filename, ".csv"), "w");
    for (int i = 0; i < quantity; i++) {
        Process* process = processes[i];
        char** process_info = malloc(6 * sizeof(char*));
        process_info[0] = calloc(33, sizeof(char));
        strcpy(process_info[0], process->name);
        process_info[1] = calloc(33, sizeof(char));
        sprintf(process_info[1], "%d", process->exec_count);
        process_info[2] = calloc(33, sizeof(char));
        sprintf(process_info[2], "%d", process->int_count);
        process_info[3] = calloc(33, sizeof(char));
        sprintf(process_info[3], "%d", process->turnaround);
        process_info[4] = calloc(33, sizeof(char));
        sprintf(process_info[4], "%d", process->response);
        process_info[5] = calloc(33, sizeof(char));
        sprintf(process_info[5], "%d", process->waiting_time);
        write_output_line(file, process_info, 6);
        for (int j = 0; j < 6; j++) {
            free(process_info[j]);
        }
        free(process_info);
    }
    fclose(file);
}

// Free memory from all processes
void destroy_processes(Process** processes, int quantity) {
    for (int i = 0; i < quantity; i++) {
        free(processes[i]->name);
        free(processes[i]);
    }
    free(processes);
}

// Generate queues
Queue** create_queues(int n) {
    Queue** queue_array = malloc(n * sizeof(Queue*));
    for (int i = 0; i < n; i++) {
        queue_array[i] = malloc(sizeof(Queue));
        queue_array[i]->head = NULL;
        queue_array[i]->tail = NULL;
        queue_array[i]->length = 0;
        queue_array[i]->priority = n - (1 + i);
    }
    return queue_array;
}

// Push new process to the queue
void push(Queue* queue, Process* process) {
    if (!queue->length) {
        queue->head = process;
        queue->tail = process;
        queue->tail->next = NULL;
    } else {
        Process* old_tail = queue->tail;
        old_tail->next = process;
        process->next = NULL;
        queue->tail = process;
    }
    queue->length++;
}

// Merge two queues
void merge(Queue* from, Queue* to) {
    if (!to->length) {
        to->tail = from->tail;
        to->head = from->head;
    } else {
        to->tail->next = from->head;
        to->tail = from->tail;
    }
    Process* iterator = from->head;
    while (iterator != NULL) {
        iterator->current_queue = 0;
        iterator = iterator->next;
    }
    to->length += from->length;
    from->head = NULL;
    from->tail = NULL;
    from->length = 0;
}

// Pop new process from the queue
Process* pop(Queue* queue) {
    if (queue->length) {
        queue->length--;
        Process* first = queue->head;
        queue->head = queue->head->next;
        if (!queue->length) {
            queue->tail = NULL;
        }
        return first;
    } else {
        return NULL;
    }
}

// Free memory from all queues
void destroy_queues(Queue** queues, int quantity) {
    for (int i = 0; i < quantity; i++) {
        free(queues[i]);
    }
    free(queues);
}
