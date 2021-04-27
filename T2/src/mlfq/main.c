#include "mlfq.h"


// Scheduler Simulator
int main(int argc, char **argv) {

    // Check input
    if (argc < 6) {
        printf("[Error] Not enough arguments were provided!\n");
        return 1;
    }

    // Receive args
    InputFile* file = read_file(argv[1]);
    int k = file->len;
    int n_queues = atoi(argv[3]);
    int q = atoi(argv[4]);
    int S = atoi(argv[5]);

    // Initialize program
    Process** process_array = load_processes(file);
    input_file_destroy(file);
    Queue** queue_array = create_queues(n_queues);
    int t = 0;
    int index = 0;
    int finished_counter = 0;
    Process* cpu_exec = NULL;

    // Simulation
    while (finished_counter < k) {
        //printf("\n*** Tick: %d, S count: %d, Finished: %d/%d ***\n\n", t, S, finished_counter, k);

        // Start new processes
        bool loading = true;
        while (loading && index < k) {
            if (process_array[index]->start_time == t) {
                push(queue_array[0], process_array[index]);
                //printf("-> New Process: %s\n", process_array[index]->name);
                index++;
            } else {
                loading = false;
            }
        }

        // Add waiting time & check processes in waiting
        for (int p = 0; p < k; p++) {
            Process* current_process = process_array[p];
            if (current_process->start_time < t && !current_process->finished && !current_process->executing) { // Add waiting time
                current_process->waiting_time++;
            }
            if (current_process->waiting) { // Check processes in waiting
                current_process->wait--;
                if (!current_process->wait) {
                    current_process->waiting = false;
                    current_process->wait = current_process->initial_wait;
                    queue_array[current_process->current_queue]->length++;
                    //printf("-> Activated: %s (Wait time %d)\n", current_process->name, current_process->wait);
                }
            }
        }

        // CPU is busy
        if (cpu_exec != NULL) {
            Process* exec_process = cpu_exec;
            exec_process->cycles--;
            exec_process->quantum--;
            if (exec_process->initial_wait) {
                exec_process->wait--;
            }
            if (!exec_process->cycles) { // Process is finished
                finished_counter++;
                exec_process->executing = false;
                exec_process->finished = true;
                exec_process->turnaround = t - exec_process->start_time;
                if (!exec_process->quantum) {
                    exec_process->int_count++;
                    //printf("-> Interrupted: %s (Finished)\n", exec_process->name);
                }
                cpu_exec = NULL;
                //printf("-> Finished: %s\n", exec_process->name);
            } else if (!exec_process->quantum) { // Process interruption
                if (exec_process->current_queue < n_queues - 1) {
                    exec_process->current_queue++;
                }
                exec_process->int_count++;
                exec_process->executing = false;
                //printf("-> Interrupted: %s (Wait time %d)\n", exec_process->name, exec_process->wait);
                if (exec_process->initial_wait && !exec_process->wait) {
                    exec_process->wait = cpu_exec->waiting_delay;
                    exec_process->waiting = true;
                    //printf("-> Waiting: %s (Sleep time %d)\n", exec_process->name, exec_process->wait);
                }
                cpu_exec = NULL;
                push(queue_array[exec_process->current_queue], exec_process);
            } else if (exec_process->initial_wait && !exec_process->wait) { // Process I/O
                if (exec_process->current_queue) {
                    exec_process->current_queue--;
                }
                exec_process->executing = false;
                exec_process->wait = cpu_exec->waiting_delay;
                exec_process->waiting = true;
                cpu_exec = NULL;
                push(queue_array[exec_process->current_queue], exec_process);
                //printf("-> Waiting: %s (Sleep time %d)\n", exec_process->name, exec_process->wait);
            }
        }

        // Debug queues & CPU
        /*
        if (cpu_exec != NULL) {
            printf("\n * Running: %s\n", cpu_exec->name);
        } else {
            printf("\n * Running: None\n");
        }
        printf("\n");
        for (int i = 0; i < n_queues; i++) { // Get top queue that contains processes
            Process* it = queue_array[i]->head;
            printf(" [");
            while (it != NULL) {
                printf("%s", it->name);
                if (it->waiting) {
                    printf("(W)");
                }
                if (it->next != NULL) {
                    printf(", ");
                }
                it = it->next;
            }
            printf("]\n");
        }
        */

        // CPU can attend new process
        if (cpu_exec == NULL) {
            Queue* queue = NULL;
            for (int i = 0; i < n_queues; i++) { // Get top queue that contains processes
                if (queue == NULL && queue_array[i]->length) {
                    queue = queue_array[i];
                }
            }
            if (queue != NULL) { // READY is not empty
                Process* selected_process = pop(queue);
                selected_process->quantum = (n_queues - queue->priority) * q;
                selected_process->exec_count++;
                selected_process->executing = true;
                if (selected_process->response == -1) {
                    selected_process->response = t - selected_process->start_time;
                }
                cpu_exec = selected_process; // Process enters the CPU
                //printf("\n-> Selected for CPU: %s (Wait: %d, Cycles: %d, Quantum: %d)\n", selected_process->name, selected_process->wait, selected_process->cycles, selected_process->quantum);
            }
        }

        // Reset all queues
        if (!S) {
            S = atoi(argv[5]);
            for (int i = 1; i < n_queues; i++) {
                merge(queue_array[i], queue_array[0]);
            }
            //printf("\n-> Merged all queues!\n");
        }

        // Next tick
        S--;
        t++;
    }

    // Write output & free memory
    destroy_queues(queue_array, n_queues);
    char* output_name = argv[2];
    write_output(process_array, k, output_name);
    destroy_processes(process_array, k);

    return 0;
}
